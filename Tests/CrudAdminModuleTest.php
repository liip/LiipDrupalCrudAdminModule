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
                "Function get${moduleName} does not exist." . PHP_EOL,
                "Function get${entityName}ById does not exist." . PHP_EOL,
                "Function get${entityName}s does not exist." . PHP_EOL,
                "Function delete${entityName} does not exist." . PHP_EOL,
                "Function submitHandler does not exist." . PHP_EOL,
                "Entity ($entityName) does not implement mandatory interface " . '(\Liip\Drupal\Modules\CrudAdmin\Entity\EntityInterface).' . PHP_EOL
            ))
        );

        $dcc = $this->getDrupalCommonConnectorMock();
        $dcm = $this->getDrupalModuleConnectorMock(array('module_hook'));
        $dcm
            ->expects($this->exactly(7))
            ->method('module_hook')
            ->will($this->returnValue(null));

        $factory = $this->getDrupalConnectorFactoryMock(array('getModuleConnector', 'getCommonConnector'));
        $factory
            ->staticExpects($this->once())
            ->method('getCommonConnector')
            ->will($this->returnValue($dcc));
        $factory
            ->staticExpects($this->once())
            ->method('getModuleConnector')
            ->will($this->returnValue($dcm));

        $this->setExpectedException('LogicException');

        _drupalcrudadminmodule_verify_contract($moduleName, $entityName, $factory);
    }

    public function testVerifyContractExpectWarning()
    {

        $moduleName = 'specialtestmodule';
        $entityName = 'entityforspecialtestmodule';
        $message = 'Related module (specialtestmodule) does not implement some optional functions. Missing functions: generateEditForm, generateOverviewTable' . PHP_EOL;

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
            ->expects($this->exactly(7))
            ->method('module_hook')
            ->will($this->onConsecutiveCalls(
                null,
                null,
                true,
                true,
                true,
                true,
                true
            ));

        $factory = $this->getDrupalConnectorFactoryMock(array('getModuleConnector', 'getCommonConnector'));
        $factory
            ->staticExpects($this->once())
            ->method('getCommonConnector')
            ->will($this->returnValue($dcc));
        $factory
            ->staticExpects($this->once())
            ->method('getModuleConnector')
            ->will($this->returnValue($dcm));

        _drupalcrudadminmodule_verify_contract($moduleName, $entityName, $factory);
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
}

class entityforspecialtestmodule implements EntityInterface
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
