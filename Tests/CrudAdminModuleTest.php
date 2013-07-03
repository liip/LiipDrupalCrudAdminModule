<?php
use Liip\Drupal\Modules\drupalcrudadminmodule\Tests\CrudAdminModuleTestCase;


class CrudAdminModuleTest extends CrudAdminModuleTestCase
{
    public function testRegisterCallback()
    {
        $dcc = $this->getDrupalCommonConnectorMock();
        $assertions = $this->getAssertionObjectMock();

        drupalcrudadminmodule_registerCallback('submit', 'TuxCallback', $dcc, $assertions);
    }
}
