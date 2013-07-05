<?php

use Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface;
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
 * Verifies that the related module provides the mandatory functions.
 *
 * @param string $moduleName
 * @param string $entityName
 *
 * @throws LogicException in case the target module does not implement the mandatory functions.
 */
function _drupalcrudadminmodule_verify_contract($moduleName, $entityName)
{
    /** @var \Liip\Drupal\Modules\DrupalConnector\Module $dcm */
    $dcm = ConnectorFactory::getModuleConnector();

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

    if (in_array(class_implements($entityName), array('\Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface'))) {
        $error[] = "Entity ($entityName) does not implement mandatory interface ".
            '(\Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface.' . PHP_EOL;
    }

    // this function is not mandatory if the base entity schema is suitable to generate the overview table.
    if (!$dcm->module_hook($moduleName, "generateOverviewTable")) {
        $warning[] = "Function delete${entityName} does not exist." . PHP_EOL;
    }

    if (!empty($warning)) {
        $dcc->watchdog(
            $moduleName,
            sprintf(
                'Related module (%s) does not implement some optional functions. Missing functions: %s' . PHP_EOL,
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
                implode(', ', $error)
            )
        );
    }
}

/**
 * Gathers necessary information to display a set of entities as HTML table.
 *
 * @param string $moduleName
 * @param EntityInterface $entity
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


    // todo: since the header fields will differ when the custom hook was used, the header should be generated from
    //       the $rows array (keys identify the header fields)
    $variables = array(
        'header' => array(
            $dcc->t('Title'),
            $dcc->t('Description'),
            $dcc->t('actions'),
        ),
        'empty'  => $dcc->t('LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY', array('@entity' => $entityName)),
        'rows'   => $rows,
    );

    $form['overview'] = array(
        '#markup' => $dct->theme('table', $variables),
    );

    return $form;
}
