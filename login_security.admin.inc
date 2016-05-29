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
    '#title' => t('Track time'),
    '#default_value' => config_get('login_security.settings', 'login_security_track_time'),
    '#element_validate' => array('_login_security_valid_integer'),
    '#size' => 3,
    '#description' => t('The time window to check for security violations: the time in hours the login information is kept to compute the login attempts count. An example to align with PCI-DSS 3.2 would be 30 minutes. After that time, the attempt is deleted from the list, and will never be considered again.'),
    '#field_suffix' => t('Minutes'),
  );
  $form['login_security_user_wrong_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of login failures before blocking a user'),
    '#default_value' => config_get('login_security.settings', 'login_security_user_wrong_count'),
    '#element_validate' => array('_login_security_valid_integer'),
    '#size' => 3,
    '#description' => t('Enter the number of login failures a user is allowed. After this amount is reached, the user will be blocked, no matter the host attempting to log in. Use this option carefully on public sites, as an attacker may block your site users. The user blocking protection will not disappear and should be removed manually from the <a href="!user">user management</a> interface.', array('!user' => '/admin/people')),
    '#field_suffix' => t('Failed attempts'),
  );

  $form['login_messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notifications'),
  );
  $form['login_messages']['login_security_notice_attempts_available'] = array(
    '#type' => 'checkbox',
    '#title' => t('Notify the user about the number of remaining login attempts'),
    '#default_value' => config_get('login_security.settings', 'login_security_notice_attempts_available'),
    '#description' => t('Checking this option, the user is notified about the number of remaining login attempts before the account gets blocked. Security tip: If you enable this option, try to not disclose as much of your login policies as possible in the message shown on any failed login attempt.'),
  );
  $form['login_messages']['login_security_user_blocked_email_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Select who should get an email message when a user is blocked by this module'),
    '#description' => t('No notification will be sent if the field is blank'),
    '#default_value' => config_get('login_security.settings', 'login_security_user_blocked_email_user'),
    '#element_validate' => array('_login_security_valid_user'),
  );
  $form['login_messages']['login_security_login_activity_email_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Select who should get an email message when an ongoing attack is detected'),
    '#description' => t('No notification will be sent if the field is blank'),
    '#default_value' => config_get('login_security.settings', 'login_security_login_activity_email_user'),
    '#element_validate' => array('_login_security_valid_user'),
  );

  if (user_access('access user profiles')) {
    $form['login_messages']['login_security_user_blocked_email_user']['#autocomplete_path'] = 'user/autocomplete';
    $form['login_messages']['login_security_login_activity_email_user']['#autocomplete_path'] = 'user/autocomplete';
  }

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('login_security_admin_settings_submit'),
  );

  // Clean event tracking list.
  $form['actions']['login_security_clean_tracked_events'] = array(
    '#type' => 'submit',
    '#value' => t('Clear event tracking information'),
    '#weight' => 20,
    '#submit' => array('_login_security_clean_tracked_events'),
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
  $config->set('login_security_activity_threshold', $form_state['values']['login_security_activity_threshold']);
  $config->set('login_security_notice_attempts_available', $form_state['values']['login_security_notice_attempts_available']);
  $config->set('login_security_user_blocked_email_user', $form_state['values']['login_security_user_blocked_email_user']);
  $config->set('login_security_login_activity_email_user', $form_state['values']['login_security_login_activity_email_user']);
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
 * Verify that element is a valid username.
 */
function _login_security_valid_user($element, &$form_state) {
  if ($element['#value'] !== '') {
    $count = db_select('users', 'u')
      ->condition('name', $element['#value'])
      ->countQuery()
      ->execute()
      ->fetchField();
    if (intval($count) != 1) {
      form_error($element, t('The @field field should be a valid username.', array('@field' => $element['#title'])));
    }
  }
}

/**
 * Clean login_security_track table.
 */
function _login_security_clean_tracked_events($form, &$form_state) {
  backdrop_set_message(t('Login Security event track list is now empty.'));
  _login_security_remove_all_events();
}
