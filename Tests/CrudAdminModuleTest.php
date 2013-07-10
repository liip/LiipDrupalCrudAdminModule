<?php
/**
 * Test suite to ensure correct functionallity of the module.
 *
 * @author     Bastian Feder <drupal@bastian-feder.de>
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 * @copyright  Copyright (c) 2013 Liip Inc.
 */

use Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface;
use Liip\Drupal\Modules\drupalcrudadminmodule\Tests\CrudAdminModuleTestCase;

class CrudAdminModuleTest extends CrudAdminModuleTestCase
{
    public function testVerifyContractExpectingException()
    {
        $moduleName = 'specialtestmodule';
        $entityName = 'stdclass';
        $message = sprintf(
            'Related module (%s) does not implement the mandatory functions ' .
            'or the entity does implement the mandatory interface. Occured errors: %s' . PHP_EOL,
            $moduleName,
            implode(', ', array(
                "Function getStdclassById does not exist." . PHP_EOL,
                "Function getStdclasss does not exist." . PHP_EOL,
                "Function deleteStdclass does not exist." . PHP_EOL,
                "Function submitHandler does not exist." . PHP_EOL,
                "Function getModuleNamspaces does not exist." . PHP_EOL,
                "Entity (\Stdclass) does not implement mandatory interface " . '(\Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface).' . PHP_EOL
            ))
        );

        $dcc = $this->getDrupalCommonConnectorMock();
        $dcm = $this->getDrupalModuleConnectorMock(array('module_hook', 'module_invoke'));
        $dcm
            ->expects($this->exactly(5))
            ->method('module_hook')
            ->will($this->returnValue(null));
        $dcm
            ->expects($this->once())
            ->method('module_invoke')
            ->will($this->returnValue(array('entities' => '')));

        $this->setExpectedException('LogicException', $message);

        _drupalcrudadminmodule_verify_existance_of_mandatory_callbacks($moduleName, $entityName, $dcc, $dcm);
    }

    public function testVerifyContractExpectWarning()
    {

        $moduleName = 'specialtestmodule';
        $message = 'Related module (specialtestmodule) does not implement some optional functions. '.
            'Missing functions: generateEditForm, generateOverviewTable, menuAccessController'.
             PHP_EOL;

        $dcc = $this->getDrupalCommonConnectorMock();
        $dcc
            ->expects($this->once())
            ->method('watchdog')
            ->with(
                $this->identicalTo('specialtestmodule'),
                $this->identicalTo($message),
                $this->isType('array'),
                $this->identicalTo(4)
            );

        $dcm = $this->getDrupalModuleConnectorMock(array('module_hook'));
        $dcm
            ->expects($this->exactly(3))
            ->method('module_hook')
            ->will($this->onConsecutiveCalls(
                null,
                null,
                null
            ));

        _drupalcrudadminmodule_verify_existance_of_optional_callbacks($moduleName, $dcc, $dcm);
    }

    /**
     * @dataProvider generalTableHeaderVariablesDataprovider
     */
    public function testGenerateTableHeaderVariables($expected, $rows, $tCount)
    {
        $dcc = $this->getDrupalCommonConnectorMock(array('t'));
        $dcc
            ->expects($this->exactly($tCount))
            ->method('t')
            ->with(
                $this->isType('string')
            )
            ->will($this->returnArgument(0));

        $entityName = 'entityforspecialtestmodule';

        $this->assertEquals($expected, _drupalcrudadminmodule_generate_table_header_variables($rows, $entityName, $dcc));
    }
    public function generalTableHeaderVariablesDataprovider()
    {
        return array(
            'empty row set' => array(
                array(
                    'header' => array('Title', 'Description', 'Actions'),
                    'empty'  => 'LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY',
                    'rows'   => array(),
                ),
                array(),
                4
            ),
            'non standard row set' => array(
                array(
                    'header' => array('Name', 'Number', 'Actions',
                    ),
                    'empty'  => 'LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY',
                    'rows'   => array(array('name' => 'Tux', 'number' => 9)),
                ),
                array(array('name' => 'Tux', 'number' => 9)),
                7
            ),
        );
    }

    /**
     * @dataProvider verifyFormDataprovider
     */
    public function testVerifyFormExpectingException($form)
    {
        $mandatory = array('moduleName', 'entityName');

        $this->setExpectedException('RuntimeException');

        _drupalcrudadminmodule_verify_form($form, $mandatory);
    }
    public function verifyFormDataprovider()
    {
        return array(
            'mandatory fields are not set' => array(array()),
            'at least one mandatory field are not set' => array(array('crud' => array('#moduleName' => 'tux'))),
        );
    }
}
