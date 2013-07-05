<?php
use Liip\Drupal\Modules\DrupalConnector\ConnectorFactory;

/**
 * Renders the entities of a module into a HTML table.
 *
 * @param string $moduleName
 * @param string $entityName
 * @param string $identifier
 * @param Liip\Drupal\Modules\DrupalConnector\ConnectorFactory $factory
 *
 * @return array
 */
function drupalcrudadminmodule_overview_page($moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    return $factory
        ->getFormConnector()
        ->drupal_get_form('drupalcrudadminmodule_overview_table', $moduleName, $entityName, $identifier, $factory);
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
function drupalcrudadminmodule_overview_table(array $form, $form_state, $moduleName, $entityName, $identifier, ConnectorFactory $factory)
{
    // make sure the 'liip.crudadmin.actions.list' event is triggered.
    $factory
        ->getModuleConnector()
        ->module_invoke(
            'drupaleventmanagermodule',
            'trigger',
            'liip.crudadmin.actions.list',
            $moduleName,
            $entityName,
            $identifier
        );

    return _drupalcrudadminmodule_generateOverviewTable($form, $moduleName, $entityName, $identifier, $factory);
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
            $dcm->module_invoke($moduleName, "delete${entityName}", $identifier);
            $dcm->module_invoke(
                'drupaleventmanagermodule',
                'trigger',
                'liip.crudadmin.actions.delete',
                $moduleName,
                $entityName,
                $identifier
            );
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
