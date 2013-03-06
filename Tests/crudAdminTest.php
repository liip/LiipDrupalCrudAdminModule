<?php
use Liip\Drupal\Modules\DrupalCrudAdminModule\Tests\CrudAdminTestCase;


class DrupalCrudAdminModuleTest extends CrudAdminTestCase
{
    public function testRegisterCallback()
    {
        $dcc = $this->getDrupalCommonConnectorMock();
        $assertions = $this->getAssertionObjectMock();

        crudAdmin_registerCallback('submit', 'TuxCallback', $dcc, $assertions);


    }
}
