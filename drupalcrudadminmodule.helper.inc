<?php

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

    $error = array();

    if (!$dcm->module_hook($moduleName, "get${entityName}ById")) {
        $error = "Function get${entityName}ById does not exist." . PHP_EOL;
    }

    if (!$dcm->module_hook($moduleName, "delete${entityName}ById")) {
        $error = "Function delete${entityName}ById does not exist." . PHP_EOL;
    }

    if (in_array(class_implements($entityName), array('\Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface'))) {
        $error = "Entity ($entityName) does not implement mandatory interface ".
                              '(\Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface.' . PHP_EOL;
    }

    if (!empty($error)) {
        throw new LogicException(
            sprintf(
                'relasted module (%s) does not implement the mandatory functions '.
                'or the entity does implement the mandatory interface. Occured errors: %s' . PHP_EOL,
                implode(', ', $error)
            )
        );
    }
}

