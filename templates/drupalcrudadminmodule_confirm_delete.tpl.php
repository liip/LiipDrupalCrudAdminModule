<h3><?php echo t('Confirm deletion');?></h3>

<div class="drupalcrudadminmodule-deletion-confirmation-wrapper">
    <div class="drupalcrudadminmodule-deletion-confirmation-description">
    <?php
        echo t(
            'Do you really want to delete organisation "@name"?',
            array('@name' => t($entity->getTitle()))
        );
    ?>
    </div>
    <div class="drupalcrudadminmodule-deletion-confirmation-action-wrapper">
        <?php
        $path = sprintf("admin/config/content/crud/%s/%s/%s/delete", $moduleName, $entityName, $entity->getId());

        echo l(
            t('Cancel'),
            $path,
            array(
                'query' => array(
                    'confirmDelete_'. $entity->getId() => 'false',
                    'destination' => $_GET['destination']
                ),
        'attributes' => array('class' => array('button'))
        )
        );

        echo l(
        t('Confirm'),
            $path,
        array(
            'query' => array(
                'confirmDelete_'. $entity->getId() => 'true',
                'destination' => $_GET['destination']
            ),
                'attributes' => array('class' => array('button'))
            )
        );
        ?>
    </div>
</div>
