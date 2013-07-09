<?php
/**
 * Created by JetBrains PhpStorm.
 * User: lapistano
 * Date: 7/9/13
 * Time: 10:39 AM
 * To change this template use File | Settings | File Templates.
 */

namespace Liip\Drupal\Modules\drupalcrudadminmodule\Tests\Fixtures;


use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;

class EntityForSpecialTestModule implements EntityInterface
{
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
}
