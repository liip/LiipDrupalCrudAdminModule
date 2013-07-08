<?php
namespace Liip\Drupal\Modules\CrudAdmin\Entities;

interface EntityInterface {

    /**
     * Provides the unique identifier of the entity.
     *
     * @return mixed
     */
    public function getId();

    /**
     * Provides the long description of an entity.
     *
     * @return string
     */
    public function getDescription();

    /**
     * Provides the name or title of the entity.
     *
     * @return string
     */
    public function getTitle();
}
