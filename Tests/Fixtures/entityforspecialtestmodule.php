<?php

use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;

class EntityForSpecialTestModule implements EntityInterface
{
    protected $id;
    protected $description;
    protected $title;

    /**
     * Provides the unique identifier of the entity.
     * @return mixed
     */
    public function getId()
    {
        return 42;
    }

    /**
     * Provides the long description of an entity.
     * @return string
     */
    public function getDescription()
    {
        return 'description of entityforspecialtestmodule';
    }

    /**
     * Provides the name or title of the entity.
     * @return string
     */
    public function getTitle()
    {
        return 'title of entityforspecialtestmodule';
    }

    /**
     * Generates an array representation of an instance of this class.
     * @return array
     */
    public function toArray()
    {
        $properties = array();

        foreach ($this as $member => $default) {

            $func = 'get' . ucfirst($member);

            if (method_exists($this, $func)) {
                $properties[$member] = $this->$func();
            }
        }

        return $properties;
    }
}
