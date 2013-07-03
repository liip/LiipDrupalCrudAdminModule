<?php
use Liip\Drupal\Modules\drupalcrudadminmodule\Tests;


class CrudAdminModuleTest extends CrudAdminTestCase
{
    public function testRegisterCallback()
    {
        $dcc = $this->getDrupalCommonConnectorMock();
        $assertions = $this->getAssertionObjectMock();

        drupalcrudadminmodule_registerCallback('submit', 'TuxCallback', $dcc, $assertions);
    }
}
