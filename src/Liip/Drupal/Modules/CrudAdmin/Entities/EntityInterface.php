<?php

/**
 * Defines the contract every entity at least has to respect to be able to interact with this module.
 *
 * @author     Bastian Feder <drupal@bastian-feder.de>
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 * @copyright  Copyright (c) 2013 Liip Inc.
 */
namespace Liip\Drupal\Modules\CrudAdmin\Entities;

interface EntityInterface {

    /**
     * Provides the unique identifier of the entity.
     *
     * @return mixed
     */
    public function getId();

    /**
     * Provides the name or title of the entity.
     *
     * @return string
     */
    public function getTitle();
}
