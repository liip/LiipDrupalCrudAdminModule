<?php

if (!defined('PROJECT_DIR')) {
    define('PROJECT_DIR', __DIR__ . '/../../../../..');
}

$moduleDir =  dirname(__DIR__);

if (file_exists($moduleDir . "/vendor/autoload.php")) {

    $loader = require_once $moduleDir . "/vendor/autoload.php";

} else if (file_exists(PROJECT_DIR . "/vendor/autoload.php")) {

    $loader = require_once PROJECT_DIR . "/vendor/autoload.php";

} else {
    die(
        "\n[ERROR] You need to run composer before running the test suite.\n".
        "To do so run the following commands:\n".
        "    curl -s http://getcomposer.org/installer | php\n".
        "    php composer.phar install --dev\n\n"
    );
}

$loader->addClassMap(
    array(
        'netmigrosintranet\modules\DrupalCrudAdminModule\Tests\CrudAdminTestCase' => $moduleDir . '/Tests/crudAdminTestCase.php',
    )
);

// unfortunately procedural stuff is not supported by composer autoloader
require_once($moduleDir . '/crudAdmin.module');
