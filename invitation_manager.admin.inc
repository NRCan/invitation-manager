<?php

/**
 * Form that is loaded to create the settings page
 */
function invitation_manager_admin() {
  $form = array();

  $form['admin'] = array(
      '#type' => 'fieldset',
      '#title' => t('Invitation Manager Settings'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
  );

  $enable_options = array(
    'on' => t('Enabled'),
    'off' => t('Disabled'),
  );

  $form['admin']['invitation_manager_enabled'] = array(
    '#type' => 'radios',
    '#title' => t('Invitation Manager'),
    '#options' => $enable_options,
    '#default_value' => variable_get('invitation_manager_enabled', 'off'),
  );

  // Per-role visibility.
  $role_options = array_map('check_plain', user_roles());
  $form['admin']['invitation_manager_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Limit Invitation Manager for specific roles'),
    '#default_value' => variable_get('invitation_manager_roles', array()),
    '#options' => $role_options,
    '#description' => t('Include scripts only for the selected role(s). If you select no roles, the scripts will be included for all users.'),
  );

  $form['admin']['invitation_manager_token'] = array(
    '#type' => 'radios',
    '#title' => t('Enable time token on local menu path'),
    '#options' => $enable_options,
    '#default_value' => variable_get('invitation_manager_token', 'on'),
  );

  $form['admin']['invitation_manager_filemode'] = array(
    '#type' => 'radios',
    '#title' => t('On save use module files or pull from github'),
    '#options' => array(
      'local' => t('Local Files'),
      'remote' => t('Download current from <a href="https://github.com/ServiceCanada/invitation-manager">github</a>'),
    ),
    '#default_value' => variable_get('invitation_manager_filemode', 'local'),
    '#description' => t('The github files will alaways be the most current. Re-save this page to update the included files.'),
  );

  return system_settings_form($form);
}

function invitation_manager_admin_validate($form, $form_state) {
}

/**
 * Form that is loaded to create the settings page
 */
function invitation_manager_surveys($form, &$form_state) {
  $form = array();

  $form['config'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manage the configuration file'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
  );

  $enabled = variable_get('invitation_manager_enabled', 'on');
  if ($enabled != 'on') {
    $form['config']['note'] = array(
      '#markup' => '<p><em><strong>'.t('Note: Invitation Manager is disabled.').'</strong></em></p>'
    );
  }

  // ---
  $options = array(
    BLOCK_VISIBILITY_NOTLISTED => t('All pages except those listed'),
    BLOCK_VISIBILITY_LISTED => t('Only the listed pages'),
  );
  $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

  $form['config']['invitation_manager_visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show block on specific pages'),
    '#options' => $options,
    '#default_value' => variable_get('invitation_manager_visibility', BLOCK_VISIBILITY_NOTLISTED),
  );
  $form['config']['invitation_manager_pages'] = array(
    '#type' => 'textarea',
    '#title' => '<span class="element-invisible">' . t('Pages') . '</span>',
    '#default_value' => variable_get('invitation_manager_pages', ''),
    '#description' => $description,
  );

  $imjson = variable_get('invitation_manager_imjson');
  if (empty($imjson) || strlen($imjson) < 24) {
    $imjson = invitation_manager_get_default_imjson();
  }

  $form['config']['invitation_manager_imjson'] = array(
    '#type' => 'textarea',
    '#title' => t('im.json configuration file'),
    '#default_value' => $imjson,
    '#rows'  => 50,
    '#description' => t('If needed, the default file and documentation are in the git repo: https://github.com/ServiceCanada/invitation-manager. You can use the JSONlint page here to validate your configuration file https://jsonlint.com/.'),
  );

  $form['config']['documentation'] = array(
    '#markup' => <<< HTML
    <p>To test the site. Go to a page you think it should work on and add testing parameters to the URL like this:<br>
      Scope Site: https://www.dev.nrcan.gc.ca/energy-efficiency-homes/20546?logim=1&im_scope=site&im_nocookiecheck=1&im_nodatecheck=1 or<br>
      Scope Page: https://www.dev.nrcan.gc.ca/energy-efficiency-homes/20546?logim=1&im_scope=page&im_nocookiecheck=1&im_nodatecheck=1
  </p>
HTML
  );


  $form = system_settings_form($form);

  // Save the saved file to disk
  $form['#submit'][] = 'invitation_manager_surveys_save_files';

  return $form;
}

function invitation_manager_surveys_validate($form, $form_state) {
}

function invitation_manager_surveys_save_files($form, $form_state) {
  $file_contents = !empty($form_state['values']['invitation_manager_imjson'])? $form_state['values']['invitation_manager_imjson'] : FALSE;
  $date = 'backups/'.date('Y-m-d_G-i-s');
  if (!empty($file_contents)) {
    $file_destination = 'public://invitation_manager/';
    $file_path = 'im.json';

    $backups_folder = 'public://invitation_manager/backups';
    if (!file_prepare_directory($backups_folder, FILE_CREATE_DIRECTORY)) {
      watchdog('invitation_manager', 'Failed to create invitation_manager backups directory: %dirpath', array('%dirpath' => $library_folder_directory), WATCHDOG_ERROR);
      return FALSE;
    }
    //$file_contents = check_markup($file_contents, 'full_html', $lang, FALSE);

    // Save both the new file and a timestamped backup
    $result = file_unmanaged_save_data($file_contents, $file_destination.$file_path, FILE_EXISTS_REPLACE);
    $result = file_unmanaged_save_data($file_contents, $file_destination.$date.'_'.$file_path);
  }

  // Save/Update other files.
  invitation_manager_prep_library();

}
