# Liip Drupal Crud Admin Module
The idea behind this module is to provide a most forward standard implementation to handle CRUD operations.
This is done by providing a base module to be used by other modules providing functions to persist and read data from a data source.
»Most forward« does mean that if several expectations are met this module will take care of every CRUD operation it self.
Further it should provide a number of custom hooks to replace or enhance those operations.
In addition it emits events after every major action (like: create, read, update, delete, and list) passing the complete scope
of the current operation.


**NOT USEABLE YET**


## Current Travis Status

[![Build Status](https://secure.travis-ci.org/liip/drupalcrudadminmodule.png?branch=master)](http://travis-ci.org/liip/drupalcrudadminmodule)


## Installation
The source is now PSR-0 compatible. There is no specific installation routine to be followed. Just clone or checkout the source into to your project
and use it.
In case you don't use a [PSR-0](https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-0.md) compatible autoloader, you only have to add the `bootstrap.php` into your bootstrap or
autoloader.

### Composer
Add the following lines to your `composer.json` file and update your project's composer installation.

```json
{
    "require": {
       "liip/drupalcrudadminmodule": "dev-master"
    }
}
```

This composer configuration will checkout the 'cutting eadge' version ('dev-master') of the project. Be alarmed that this might be broken sometimes.


**NOTE:**
In case you do not know what this means the [composer project website](http://getcomposer.org) is a good place to start.


### Github
Thus I recommend the composer way to make liip/drupalcrudadminmodule a dependency to your project.
The sources are also available via github. Just clone it as you might be familiar with.

```bash
$ git clone git://github.com/liip/liipdrupalcrudadminmodule.git
```

## Dependencies

- LiipDrupalConnector (http://github.com/liip/liipdrupalconnector)
- LiipDrupalRegistryModule (http://github.com/liip/liipdrupalregistrymodule)
- Assert (http://github.com/beberlei/assert)

## Usage

### Mandatory custom hooks
To not have to deal with the ability to store and retrieve data or how to proceed in case of a form submit a number of
mandatory hooks were defined. These hook have to be provided by the consuming module.
The existence of these modules are verified by the drupalcrudadminmodule.

#### hook_get<EntityName>s()
This hook shall provide a list of entities fetched from whatever data source. It also has to make sure that the involved
entities meet the expectations the interface Liip\Drupal\Modules\CrudAdmin\Entities\EntityInterface defines.
This is to ensure the correct behavior of an entity when it comes to the generation of an overview table.

E.g.

    $organisations = module_invoke('organisationsmanager', 'getOrganisations');

#### hook_get?<EntityName>ById()
Expecting the unique identifier of an entity it returns either a preset entity or in case the entity could not be found
an exception.
sure the correct behavior of an entity when it comes to the generation of an overview table.

E.g.

    $organisation = module_invoke('organisationsmanager', 'getOrganisationById', $uniqueId);

#### hook_delete<EntityName>()
This hook will be invoked by the handler of the delete action.
Its general purpose to actually perform the deletion of the entity identifies by the given unique id.

E.g.

    $suceed = module_invoke('organisationsmanager', 'deleteOrganisation', $uniqueId);

#### hook_submitHandler()
The submit handler is the only custom hook actually injecting functionality to the crud admin module not being altered
in any case.
It replaces the hook_form_submit() of the consuming module and should be implemented in the same way.

#### hook_getModuleNamspaces()
This custom hook shall provide a set of namespaces defined for used entities, registry, and the base name space of the module.

e.g.

```php
function druapalsrudadminmodule_getModuleNamspaces()
{
    return array(
        'base' => '\\Liip\\Drupal\\Modules\\CrudAdmin',
        'entities' => '\\Liip\\Drupal\\Modules\\CrudAdmin\\Classes\\Entities',
    );
}
```

### Optional custom hooks
Optional custom hook have a working repesentation in the curdadminmodule. The following hooks will be called on different
occursions within the process. In case they are not present in the consuming module or do not return the expected format
a default implementation will take over providing ome kind of a fallback solution.

#### hook_generateEditForm()
This hook shall generate the form array representing the edit form of the current entity. Remember the entity is defined
by the consuming module and may differ from the default. In that case the hook defines the alternative form to be used to
add and modify an entity.
If not implemented a default form only covering the title and the description of an entity will be generated.

Additionaly to either the default or the custom form an extra set of fields will be added to the form defining the scope
of the current action. The fields are:

- the module name
- the entity name
- the identifier in case the form is used to modify the entity.

Do not remove nor modify these fields since they are mandatory for the process.

#### hook_generateOverviewTable()
Once implemented, this optional hook shall provide an set of table rows representing the data of an entity to be presented
and suitable for the table template (https://api.drupal.org/api/drupal/includes!theme.inc/function/theme_table/7) provided
by Drupal 7 with the difference, that the actions column (defininge the actions the entity implements, like delete or edit)
must not be added. This will be taken care of by the base module since there are a number of links to be created consistantly.
The base module does recognize if there is no entity information to be shown and displays a special message instead.
In case you want to change this message, use the Drupal administration to change it. Search for

    LIIP_DRUPAL_CRUD_ADMIN_MODULE_NO_ENTITY

but do not remove the ```%entity``` placeholder if you still want the entity name to be on display.

This hook can also be used to restrict the set of shown entities e.g. based on a permission.

#### hook_menuAccessController()
This custom hook will be used to determine if a special route is accessible for the currently logged in user or for
whatever reason in particular.

### Changing default phrases
The used phrases are registered in the Drupal translation system and may be changed by using the interface provided by
the Drupal administration backend. To find used translations search for:

    LIIP_DRUPAL_CRUD_ADMIN_MODULE

