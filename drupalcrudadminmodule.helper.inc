<?php

use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;
use Liip\Drupal\Modules\DrupalConnector\Common;
use Liip\Drupal\Modules\DrupalConnector\ConnectorFactory;


/**
 * Provides the url to be returned to after successful form submit or cancel
 *
 * @param string $moduleName
 *
 * @return string
 */
function _drupalcrudadminmodule_getDestination($moduleName)
{
    return !empty($_GET['destination']) ?
        $_GET['destination']
        : 'admin/config/content/' . $moduleName;
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
        $message[] = 'Entire section ("crud") is missing.';
    } else {

        foreach ($mandatory as $field) {

            if (empty($form['crud']['#' . $field])) {
                $message[] = "No $field set in form.";
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

    $error = array();
    $warning = array();

    if (!$dcm->module_hook($moduleName, "get${entityName}ById")) {
        $error[] = "Function get${entityName}ById does not exist." . PHP_EOL;
    }

    if (!$dcm->module_hook($moduleName, "get${entityName}s")) {
        $error[] = "Function get${entityName}s does not exist." . PHP_EOL;
    }

    if (!$dcm->module_hook($moduleName, "delete${entityName}")) {
        $error[] = "Function delete${entityName} does not exist." . PHP_EOL;
    }

    if (!$dcm->module_hook($moduleName, "get${moduleName}")) {
        $error[] = "Function get${moduleName} does not exist." . PHP_EOL;
    }

    if (!$dcm->module_hook($moduleName, "submitHandler")) {
        $error[] = "Function submitHandler does not exist." . PHP_EOL;
    }

    if (in_array(class_implements($entityName), array('\Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface'))) {
        $error[] = "Entity ($entityName) does not implement mandatory interface ".
            '(\Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface).' . PHP_EOL;
    }

    // the following function are not mandatory since this module covers for them using default implementations.
    if (!$dcm->module_hook($moduleName, "generateEditForm")) {
        $warning[] = "generateEditForm";
    }

    if (!$dcm->module_hook($moduleName, "generateOverviewTable")) {
        $warning[] = "generateOverviewTable";
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
            WATCHDOG_WARNING
        );
    }

    if (!empty($error)) {
        throw new LogicException(
            sprintf(
                'Related module (%s) does not implement the mandatory functions '.
                'or the entity does implement the mandatory interface. Occured errors: %s' . PHP_EOL,
                $moduleName,
                implode(', ', $error)
            )
        );
    }
}

/**
 * Gathers necessary information to display a set of entities as HTML table.
 *
 * @param string $moduleName
 * @param \Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface $entity
 *
 * @return array
 *
 * @todo: use callback function to delegate generation to target module
 */
function _drupalcrudadminmodule_create_entity_table_row($moduleName, EntityInterface $entity)
{
    $destination = _drupalcrudadminmodule_getDestination($moduleName);

    $row = array();
    $rows[$entity->getId()]['title'] = $dcc->t($entity->getTitle());
    $rows[$entity->getId()]['description'] = $dcc->t($entity->getDescription());
    $rows[$entity->getId()]['actions'] = sprintf(
        '<div class="table-item-action-wrapper">%s %s</div>',
        $dcc->l(
            $dcc->t('edit'),
            sprintf(
                'admin/config/content/%s/%s/%s/edit',
                $moduleName,
                $entity->getTitle(),
                $entity->getId()
            ),
            array(
                'query' => array('destination' => $destination)
            )
        ),
        $dcc->l(
            $dcc->t('delete'),
            sprintf(
                'admin/config/content/%s/%s/%s/delete',
                $moduleName,
                $entity->getTitle(),
                $entity->getId()
            ),
            array(
                'query' => array('destination' => $destination)
            )
        )
    );

    return $row;
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
 * @param ConnectorFactory $factory
 *
 * @return array
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
    if (empty($rows)) {

        $rows = array();

        // fetch all active entities
        $entities = $dcm->module_invoke($moduleName, "get${entityName}s", $identifier);

        if (!empty($entities)) {

            /** @var Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface $entity */
            foreach ($entities as $registrationKey => $entity) {
                $rows[] = _drupalcrudadminmodule_create_entity_table_row($moduleName, $entity);
            }
        }
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
 * Generates the variable set to be used by the table theme.
 *
 * @param array $rows
 * @param string $entityName
 * @param Common $dcc
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

        foreach (array_keys($rows[0]) as $columnTitle) {
            $header[] = $dcc->t(ucfirst($columnTitle));
        }

        $header[] = $dcc->t('Actions');
    }

    return  array(
        'header' => $header,
        'empty'  => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY', array('@entity' => $entityName)),
        'rows'   => $rows,
    );
}

/**
 * Generates the form to be shown when the entity shall be modufied or a new entity is to be added.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param ConnectorFactory $factory
 *
 * @return array
 */
function _drupalcrudaminmodule_generate_modification_form($moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Common $dcc */
    $dcc = $factory->getCommonConnector();

    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = $factory->getModuleConnector();

    $form = array();

    $entity = $dcm->module_invoke($moduleName, 'get'.$entityName, $identifier);

    if (empty($entity)) {
        $form['#markup'] = 'NO ENTITY FOUND!';
        $form['#redirect'] = _drupalcrudadminmodule_getDestination();

        return $form;
    }

    $form['title'] = array(
        '#type'          => 'textfield',
        '#size'          => 60,
        '#maxlenth'      => 200,
        '#title'         => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_ADD_ENTITY_TITLE'),
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
        '#default_value' => (!empty($entity->title) ? $entity->title : ''),
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
        '#default_value' => (!empty($entity->description) ? $entity->description : ''),
    );

    $form['registrationkey'] = array(
        '#type'          => 'hidden',
        '#value'         => (!empty($entity->id) ? $entity->id : ''),
        '#default_value' => (!empty($entity->id) ? $entity->id : ''),
    );

    $form['submit'] = array(
        '#type'             => 'submit',
        '#value'            => $dcc->t('Save'),
        '#submit'           => array('drupalcrudadminmodule_edit_form_submit'),
        '#connectorFactory' => $factory,
    );

    $form['cancel'] = array(
        "#markup" => $dcc->l($dcc->t('Cancel'), _drupalcrudadminmodule_getDestination())
    );

    $form['#redirect'] = _drupalcrudadminmodule_getDestination();

    return $form;
}
