<?php

/**
 * @file
 * Functionality and helper functions for Node Load Img administration.
 */

/**
 * Form builder function for the general Node load img configuration
 */
function node_load_img_admin_settings() {
    $types = node_type_get_types();
    $styles = image_styles();
    $form = array();
    $styles_form= array();
    foreach($styles as $style) {
        $styles_form[$style['name']] = isset($style['label']) ? $style['label'] : $style['name'];
    }
    
    $hidden = array();
    
    foreach($types as $type) {
        $hidden[$type->type] = array();
        $form['node_load_img_' . $type->type] = array(
            '#type' => 'fieldset',
            '#title' => $type->name,
        );
        $fields = field_info_instances('node', $type->type);
        foreach($fields as $field) {
            $field_info = field_info_field($field['field_name']);
            if($field_info['type'] === 'image') {
                $hidden[$type->type][] = $field['field_name'];
                $default = variable_get('node_load_img_' . $type->type, array());
                $form['node_load_img_' . $type->type]['node_load_img_' . $type->type . '_' . $field['field_name']] = array(
                    '#title' => $field['label'],
                    '#type' => 'checkboxes',
                    '#options' => $styles_form,
                    '#default_value' => isset($default[$field['field_name']]) ? drupal_map_assoc($default[$field['field_name']], array()) : array(),
                );
            }
        }
    }
    
    // Unset types without any image field
    foreach($hidden as $type => $fields) {
        if(empty($fields)) {
            unset($form['node_load_img_' . $type]);
        }
    }
    $form['types_and_fields'] = array(
        '#type' => 'hidden',
        '#value' => serialize($hidden)
    );
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    
    return $form;
    
}


function node_load_img_admin_settings_submit($form, &$form_state) {
    $types = unserialize($form_state['values']['types_and_fields']);
    variable_set('node_load_img', $types);
    foreach($types as $type => $fields) {
        if(!empty($fields)) {
            $styles = array();
            foreach($fields as $field)
                if(isset($form_state['values']['node_load_img_' . $type . '_' . $field])) {
                    foreach($form_state['values']['node_load_img_' . $type . '_' . $field] as $style) {
                        if($style) {
                            $styles[$field][] = $style;
                        }
                    }
                }
            variable_set('node_load_img_' . $type, $styles);
        }
    }
}