<?php
/**
 * @file
 * This module file introduces mandatory functions to handle administrative tasks.
 *
 * @author     Bastian Feder <drupal@bastian-feder.de>
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 * @copyright  Copyright (c) 2013 Liip Inc.
 */

use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;
use Liip\Drupal\Modules\DrupalConnector\ConnectorFactory;
use Liip\Drupal\Modules\DrupalConnector\Module;

/**
 * Renders the entities of a module into a HTML table.
 *
 * @param string $callback
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 */
function drupalcrudadminmodule_page($callback, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    return $factory
        ->getFormConnector()
        ->drupal_get_form($callback, $moduleName, $entityName, $identifier, $factory);
}

/**
 * Provides the definition of the entity overview html table.
 *
 * @param array $form
 * @param array $form_state
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param ConnectorFactory $factory
 *
 * @return array
 */
function drupalcrudadminmodule_overview_table(array $form, array $form_state, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    return _drupalcrudadminmodule_generateOverviewTable($form, $moduleName, $entityName, $identifier, $factory);
}

/**
 * Defines how the delete form shall look like.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 *
 * @see ${moduleName}_delete${entityName}()
 * @see ${moduleName}_get${entityName}ById()
 */
function drupalcrudadminmodule_delete_form($moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    $build = array();

    if (isset($_GET['confirmDelete_' . $identifier])) {

        if ('true' == $_GET['confirmDelete_' . $identifier]) {
            $dcm->module_invoke($moduleName, "delete".ucfirst($entityName), $identifier, $factory);
        }

        // this should send you back to where you came from.
        $dcc->drupal_goto(_drupalcrudadminmodule_getDestination($moduleName, $entityName));

    } else {

        /** @var EntityInterface $entity */
        $entity = $dcm->module_invoke($moduleName, "get".ucfirst($entityName)."ById", $identifier);

        // show confirmation page before deleting entry
        $build['#theme'] = 'drupalcrudadminmodule_confirm_delete';
        $build['#moduleName'] = $moduleName;
        $build['#entityName'] = $entityName;
        $build['#entity'] = $entity;
    }

    return $build;
}

/**
 * Provides the form to add/edit a entity.
 *
 * @param array $form
 * @param array $form_state
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param ConnectorFactory $factory
 *
 * @return array
 *
 * @see ${moduleName}_generateEditForm((str) $identifier)
 */
function drupalcrudadminmodule_edit_form(array $form, array $form_state, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var Module $dcm */
    $dcm = $factory->getModuleConnector();

    // ask target module to generate form to modify the entity.
    $moduleForm = $dcm->module_invoke($moduleName, "generateEditForm", $identifier);

    if (empty($moduleForm)) {
        $moduleForm = _drupalcrudaminmodule_generate_modification_form($moduleName, $entityName, $identifier, $factory);
    }

    $form['crud'] = array(
        '#type' => 'hidden',
        '#moduleName' => $moduleName,
        '#entityName' => $entityName,
        '#identifier' => $identifier
    );

    return array_merge($form, $moduleForm);
}

/**
 * Defines the actions to be process when the modification form was submitted.
 *
 * In case the consuming module does implement the custom hook (hook_submitHandler()) this will be invoked instead.
 * Implements the hook_form_submit().
 *
 * @param array $form
 * @param array $form_state
 *
 * @see _drupalcrudadminmodule_verify_form()
 * @see ${moduleName}_submitHandler()
 */
function drupalcrudadminmodule_edit_form_submit(array &$form, array &$form_state)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory */
    $factory = $form['submit']['#connectorFactory'];

    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Form $dcf */
    $dcf = $factory->getFormConnector();


    try {

        _drupalcrudadminmodule_verify_form($form, array('moduleName', 'entityName'));

    } catch (RuntimeException $e) {
        $dcf->form_set_error('crud', $e->getMessage());
    }

    // Drupal does not recognize the already set form redirect ;(
    $form_state['redirect'][] = $form['#redirect'];

    // ask target module to handle the submit action by itself.
    $dcm->module_invoke($form['crud']['#moduleName'], "submitHandler", $form, $form_state, $factory);
}
