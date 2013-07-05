<?php
use Liip\Drupal\Modules\DrupalConnector\ConnectorFactory;

/**
 * Renders the entities of a module into a HTML table.
 *
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 */
function drupalcrudadminmodule_overview_page(ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Form $dcf */
    $dcf = $factory->getFormConnector();

    return $dcf->drupal_get_form('drupalcrudadminmodule_overview_table', $factory);
}

/**
 * @param $form
 * @param $form_state
 * @param ConnectorFactory $factory
 *
 * @return array
 */
function drupalcrudadminmodule_overview_table($form, $form_state, ConnectorFactory $factory)
{

}

/**
 * Defines how the delete form shall look like.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 *
 * @return array
 */
function drupalcrudadminmodule_delete_form($moduleName, $entityName, $identifier)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = ConnectorFactory::getCommonConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = ConnectorFactory::getModuleConnector();

    $build = array();

    $entity = $dcm->module_invoke($moduleName, "get${entityName}ById", $identifier);

    if (isset($_GET['confirmDelete_' . $entity->id])) {

        if ('true' == $_GET['confirmDelete_' . $entity->id]) {
            $dcm->module_invoke($moduleName, "delete${entityName}ById", $identifier);
            $dcm->module_invoke('drupaleventmanagermodule', 'trigger', 'liip.crudadmin.actions.delete');
        }

        // this should send you back to where you came from.
        $dcc->drupal_goto(_drupalcrudadminmodule_getDestination($moduleName));

    } else {

        // show confirmation page before deleting entry
        $build['#theme'] = 'drupalcrudadminmodule_confirm_delete';
        $build['#module'] = $module;
        $build['#entity'] = $entity;
    }

    return $build;
}
