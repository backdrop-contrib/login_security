<?php

/**
 * @file
 * Configuration form for changing default settings.
 */

 /**
  * Build a form body for the configuration settings.
  */
function login_security_admin_settings() {
  $form = array();

  $form['login_security_track_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Track time (minutes)'),
    '#default_value' => config_get('login_security.settings', 'login_security_track_time'),
    '#element_validate' => array('_login_security_valid_integer'),
    '#size' => 3,
    '#description' => t('The time window to check for security violations: the time in hours the login information is kept to compute the login attempts count. An example to align with PCI-DSS 3.2 would be 30 minutes. After that time, the attempt is deleted from the list, and will never be considered again.'),
    '#field_suffix' => t('Minutes'),
  );
  $form['login_security_user_wrong_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of login failures before limiting login attempts'),
    '#default_value' => config_get('login_security.settings', 'login_security_user_wrong_count'),
    '#element_validate' => array('_login_security_valid_integer'),
    '#size' => 3,
    '#description' => t('Enter the number of login failures a user is allowed. After this amount is reached, the user will be blocked, no matter the host attempting to log in. Use this option carefully on public sites, as an attacker may block your site users. The user blocking protection will not disappear and should be removed manually from the <a href="!user">user management</a> interface.', array('!user' => '/admin/people')),
    '#field_suffix' => t('Failed attempts'),
  );

  $form['clean_events']['clean_tracked_events'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear event tracking'),
    '#description' => l('Delete all tracked events', 'login_security_clean_tracked_events'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('login_security_admin_settings_submit'),
  );

  return $form;
}

/**
 * Implements hook_submit().
 */
function login_security_admin_settings_submit($form, &$form_state) {
  $config = config('login_security.settings');
  $config->set('login_security_track_time', $form_state['values']['login_security_track_time']);
  $config->set('login_security_user_wrong_count', $form_state['values']['login_security_user_wrong_count']);
  $config->save();
  watchdog('login_security', t('Login security configuration updated.'), array(), WATCHDOG_INFO, l(t('Configure'), 'admin/config/people/login_security'));
}


/**
 * Verify that element is a positive integer value.
 */
function _login_security_valid_integer($element, &$form_state) {
  if (!ctype_alnum($element['#value']) || intval($element['#value']) < 0) {
    form_error($element, t('The @field field should be a positive integer value greater than or equal to 0.', array('@field' => $element['#title'])));
  }
}

/**
 * Form builder to confirm deletion of all login events.
 *
 * @see login_security_admin_settings()
 * @see confirm_form()
 *
 * @ingroup forms
 */
function login_security_clean_tracked_events($form, &$form_state) {
  return confirm_form(
    $form,
    t('Are you sure you want to delete all tracked login events?'),
    'admin/config/people/login_security',
    t('This action cannot be undone.'),
    t('Delete all events')
  );
}

/**
 * Form submit handler; delete an URL redirect after confirmation.
 *
 * @see redirect_delete_form()
 */
function login_security_clean_tracked_events_submit($form, &$form_state) {
  _login_security_remove_all_events();
  backdrop_set_message(t('Login security event tracking list is now empty.'));
  $form_state['redirect'] = 'admin/config/people/login_security';
}
