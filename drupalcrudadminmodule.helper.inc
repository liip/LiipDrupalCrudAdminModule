<?php
/**
 * @file
 * This module file introduces mandatory functions to handle different tasks.
 *
 * @author     Bastian Feder <drupal@bastian-feder.de>
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 * @copyright  Copyright (c) 2013 Liip Inc.
 */

use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;
use Liip\Drupal\Modules\DrupalConnector\Common;
use Liip\Drupal\Modules\DrupalConnector\ConnectorFactory;
use Liip\Drupal\Modules\DrupalConnector\Module;


/**
 * Provides the url to be returned to after successful form submit or cancel
 *
 * @param string $moduleName
 * @param string $entityName
 *
 * @group optionalCustomHooks
 * @return string
 *
 * @see ${moduleName}_getDestination()
 */
function _drupalcrudadminmodule_getDestination($moduleName, $entityName = '')
{
    // check for a custom hook_getDestination() ans if there is one, use this instead.
    $destination = ConnectorFactory::getModuleConnector()
        ->module_invoke($moduleName, 'getDestination');

    if (empty($destination)) {

        $path = (!empty($entityName)) ? $moduleName.'/'.$entityName : $moduleName;

        $destination = !empty($_GET['destination']) ?
            $_GET['destination']
            : 'admin/config/content/crud/' . $path;
    }


    return $destination;
}

/**
 * Determines, if the mandatory CRUD section is available in the form.
 *
 * @param array $form
 * @param array $mandatory
 *
 * @throws RuntimeException
 */
function _drupalcrudadminmodule_verify_form(array $form, array $mandatory)
{
    $messages = array();

    if (empty($form['crud'])) {
        $messages[] = 'Entire section ("crud") is missing.';
    } else {

        foreach ($mandatory as $field) {

            if (empty($form['crud']['#' . $field])) {
                $messages[] = "No $field set in form.";
            }
        }
    }

    if (!empty($messages)) {

        throw new RuntimeException(implode(', ', $messages));
    }
}

/**
 * Verifies that the related module provides the mandatory functions.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @throws LogicException
 */
function _drupalcrudadminmodule_verify_contract($moduleName, $entityName, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    _drupalcrudadminmodule_verify_existence_of_optional_callbacks($moduleName, $dcc, $dcm);
    _drupalcrudadminmodule_verify_existence_of_mandatory_callbacks($moduleName, $entityName, $dcc, $dcm);
}

/**
 * Verifies the existence of a set of mandatory callbacks to be defined by the consumer of the module.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param Liip\Drupal\Modules\DrupalConnector\Common $dcc
 * @param Liip\Drupal\Modules\DrupalConnector\Module $dcm
 *
 * @throws LogicException
 *
 * @see ${moduleName}_getModuleNamespaces()
 *
 */
function _drupalcrudadminmodule_verify_existence_of_mandatory_callbacks($moduleName, $entityName, Common $dcc, Module $dcm)
{

    $error = array();
    $entity = ucfirst($entityName);
    $namespaces = $dcm->module_invoke($moduleName, 'getModuleNamespaces');
    $entityFullName = $namespaces['entities'] . '\\' . $entity;

    $mandatory = array(
        "get${entity}ById",
        "get${entity}s",
        "delete${entity}",
        "submitHandler",
        "getModuleNamespaces"
    );

    foreach ($mandatory as $func) {

        if (!$dcm->module_hook($moduleName, $func)) {
            $error[] = sprintf(
                "Function %s does not exist." . PHP_EOL,
                $func
            );
        }
    }

    if (!class_exists($entityFullName) ||
        !in_array("Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface", array_keys(class_implements($entityFullName)))
    ) {
        $error[] = "Entity ($entityFullName) does not implement mandatory interface " .
            '(\Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface).' . PHP_EOL;
    }

    if (!empty($error)) {
        throw new LogicException(
            sprintf(
                'Related module (%s) does not implement the mandatory functions ' .
                'or the entity does implement the mandatory interface. Occurred errors: %s' . PHP_EOL,
                $moduleName,
                implode(', ', $error)
            )
        );
    }
}

/**
 * Verifies the existence of a set of optional call back functions maybe available in the consuming module.
 * These functions will then be used instead of the default function.
 *
 * @param string $moduleName
 * @param \Liip\Drupal\Modules\DrupalConnector\Common $dcc
 * @param \Liip\Drupal\Modules\DrupalConnector\Module $dcm
 */
function _drupalcrudadminmodule_verify_existence_of_optional_callbacks($moduleName, Common $dcc, Module $dcm)
{
    $warning = array();

    // options are not mandatory, since this module covers for them using default implementations.
    $optional = array(
        "generateEditForm",
        "generateOverviewTable",
        "menuAccessController",
    );

    foreach ($optional aS $func) {

        if (!$dcm->module_hook($moduleName, $func)) {
            $warning[] = $func;
        }
    }

    if (!empty($warning)) {
        $dcc->watchdog(
            $moduleName,
            sprintf(
                'Related module (%s) does not implement some optional functions. Missing functions: %s' . PHP_EOL,
                $moduleName,
                implode(', ', $warning)
            ),
            array(),
            WATCHDOG_INFO
        );
    }
}

/**
 * Gathers necessary information to display a set of entities as HTML table.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface $entity
 * @param Liip\Drupal\Modules\DrupalConnector\Common $dcc
 * @param Liip\Drupal\Modules\DrupalConnector\Module $dcm
 *
 * @return array
 */
function _drupalcrudadminmodule_create_entity_table_row($moduleName, $entityName, EntityInterface $entity, Common $dcc, Module $dcm)
{
    $row = array();
    $row['title'] = $dcc->t($entity->getTitle());
    $row['description'] = $dcc->t($entity->getDescription());
    $row['actions'] = _drupalcrudadminmdule_generate_entity_table_action_column($moduleName, $entityName, $entity, $dcc);

    return $row;
}

/**
 * Create the »action« column of the entity overview table.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface $entity
 * @param Liip\Drupal\Modules\DrupalConnector\Common $dcc
 *
 * @return string
 */
function _drupalcrudadminmdule_generate_entity_table_action_column($moduleName, $entityName, EntityInterface $entity, Common $dcc)
{
    $destination = _drupalcrudadminmodule_getDestination($moduleName, $entityName);

    return sprintf(
        '<div class="table-item-action-wrapper">%s %s</div>',
        $dcc->l(
            $dcc->t('edit'),
            sprintf(
                'admin/config/content/crud/%s/%s/%s/edit',
                $moduleName,
                $entityName,
                $entity->getId()
            ),
            array(
                'query' => array('destination' => $destination)
            )
        ),
        $dcc->l(
            $dcc->t('delete'),
            sprintf(
                'admin/config/content/crud/%s/%s/%s/delete',
                $moduleName,
                $entityName,
                $entity->getId()
            ),
            array(
                'query' => array('destination' => $destination)
            )
        )
    );
}

/**
 * Generates the entity overview table.
 * Tries to invoke a custom hook »hook_generateOverviewTable()« on the target module and uses their return value when
 * available.
 *
 * @param array $form
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 *
 * @see hook_generateOverviewTable()
 * @see hook_get${entityName}s()
 */
function _drupalcrudadminmodule_generateOverviewTable(array $form, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Theme $dct */
    $dct = $factory->getThemeConnector();

    // ask target module to render the overview table.
    $rows = $dcm->module_invoke($moduleName, "generateOverviewTable", $identifier);

    // in case the hook does not exist the module_invoke() will return null.
    if (is_null($rows)) {

        $rows = array();

        // fetch all active entities
        $entities = $dcm->module_invoke($moduleName, 'get' . ucfirst($entityName) . 's');

        if (!empty($entities)) {

            /** @var Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface $entity */
            foreach ($entities as $registrationKey => $entity) {
                $rows[$entity->getId()] = _drupalcrudadminmodule_create_entity_table_row($moduleName, $entityName, $entity, $dcc, $dcm);
            }
        }
    } else {
        $rows = _drupalcrudadminmodule_inject_actions_column($rows, $moduleName, $identifier, $factory);
    }

    $form['overview'] = array(
        '#markup' => $dct->theme(
            'table',
            _drupalcrudadminmodule_generate_table_header_variables($rows, $entityName, $dcc)
        ),
    );

    return $form;
}

/**
 * Injects the action column to every row of the table.
 *
 * @param array $rows
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param ConnectorFactory $factory
 *
 * @return array
 *
 * @see hook_get${entityName}ById()
 */
function _drupalcrudadminmodule_inject_actions_column(array $rows, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    // ask consumer module for available entities and their data.
    $entity = $dcm->module_invoke($moduleName, 'get' . ucfirst($entityName) . 'ById', $identifier);

    foreach ($rows as $row) {

        $row['actions'] = _drupalcrudadminmdule_generate_entity_table_action_column($moduleName, $entity, $dcc);
    }

    return $rows;
}

/**
 * Generates the variable set to be used by the table theme.
 *
 * @param array $rows
 * @param string $entityName
 * @param Liip\Drupal\Modules\DrupalConnector\Common $dcc
 *
 * @return array
 */
function _drupalcrudadminmodule_generate_table_header_variables(array $rows, $entityName, Common $dcc)
{
    $header = array(
        $dcc->t('Title'),
        $dcc->t('Description'),
        $dcc->t('Actions'),
    );

    if (!empty($rows)) {
        $header = array();
        $clone = $rows;
        $row = array_shift($clone);

        foreach (array_keys($row) as $columnTitle) {
            $header[] = $dcc->t(ucfirst($columnTitle));
        }
    }

    return array(
        'header' => $header,
        'empty'  => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY', array('%entity' => $entityName)),
        'rows'   => $rows,
    );
}

/**
 * Generates the form to be shown when the entity shall be modified or a new entity is to be added.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 *
 * @see hook_get${entityName}ById()
 */
function _drupalcrudaminmodule_generate_modification_form($moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    $form = array();
    $title = '';
    $id = '';
    $description = '';

    if (!empty($identifier)) {

        $entity = $dcm->module_invoke($moduleName, 'get' . ucfirst($entityName) . 'ById', $identifier);
        $title = $entity->getTitle();
        $id = $entity->getId();
        $description = $entity->getDescription();

        if (empty($entity)) {
            $form['#markup'] = $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY', array('%entity' => $entityName));
            $form['#redirect'] = _drupalcrudadminmodule_getDestination($moduleName, $entityName);

            return $form;
        }
    }

    $form['title'] = array(
        '#type'          => 'textfield',
        '#size'          => 60,
        '#maxlenth'      => 200,
        '#title'         => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_ADD_ENTITY_TITLE', array('%entity' => $entityName)),
        '#description'   => $dcc->t(
            'LIIP_DRUPAL_CRUD_ADMIN_MODULE_ADD_ENTITY_DESCRIPTION',
            array(
                '!admin' => $dcc->l(
                    $dcc->t('standard administration'),
                    'admin/config/regional/translate/translate'
                )
            )
        ),
        '#required'      => true,
        '#default_value' => $title,
    );

    $form['description'] = array(
        '#type'          => 'textfield',
        '#size'          => 30,
        '#maxlength'     => 60,
        '#title'         => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_ADD_ENTITY_DESCRIPTION_TITLE'),
        '#required'      => true,
        '#description'   => $dcc->t(
            'LIIP_DRUPAL_CRUD_ADMIN_MODULE_ADD_ENTITY_DESCRIPTION',
            array(
                '!admin' => $dcc->l(
                    $dcc->t('standard administration'),
                    'admin/config/regional/translate/translate'
                )
            )
        ),
        '#default_value' => $description,
    );

    $form['registrationkey'] = array(
        '#type'          => 'hidden',
        '#value'         => $id,
        '#default_value' => $id,
    );

    $form['submit'] = array(
        '#type'             => 'submit',
        '#value'            => $dcc->t('Save'),
        '#submit'           => array('drupalcrudadminmodule_edit_form_submit'),
        '#connectorFactory' => $factory,
    );

    $form['cancel'] = array(
        "#markup" => $dcc->l($dcc->t('Cancel'), _drupalcrudadminmodule_getDestination($moduleName, $entityName))
    );

    $form['#redirect'] = _drupalcrudadminmodule_getDestination($moduleName, $entityName);

    return $form;
}
