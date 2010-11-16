<?php

/**
 * Settings page callback function.
 */
function commentmail_admin_settings() {
  $form['commentmail_to'] = array(
    '#type' => 'textfield',
    '#title' => t('Send to'),
    '#default_value' => variable_get('commentmail_to', variable_get('site_mail', '')),
    '#description' => t('A mail message will be sent here when new comments are posted to the site. Separate multiple addresses with a comma.')
  );

  $form['commentmail_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Send mail for'),
    '#default_value' => variable_get('commentmail_mode', 'approval'),
    '#options' => array(
      'all' => t('all new comments'),
      'approval' => t('just comments needing approval'),
      'disable' => t('none (disabled)'),
    ),
  );

  $placeholders = t("The following placeholders are available:
<dl>
  <dt>@site</dt>
  <dd>The name of your site.</dd>

  <dt>@node</dt>
  <dd>The title of the post this comment was submitted for.</dd>

  <dt>@quick_approve</dt>
  <dd>The address the recipient can visit to view/edit/delete/approve the comment - recommended.</dd>

  <dt>@approval_url</dt>
  <dd>The address the recipient can visit to publish the comment.</dd>

  <dt>@delete_url</dt>
  <dd>The address the recipient can visit to delete the comment.</dd>

  <dt>@ban_url</dt>
  <dd>The address the recipient can visit to delete the comment and ban the user who submitted the comment.</dd>

  <dt>@edit_url</dt>
  <dd>The address the recipient can visit to edit the comment.</dd>

  <dt>@view_url</dt>
  <dd>The address the recipient can visit to view the comment.</dd>

  <dt>@admin_url</dt>
  <dd>The address of the comment moderation.</dd>

  <dt>@queue_url</dt>
  <dd>The address of the comment moderation approval queue.</dd>

  <dt>@host</dt>
  <dd>The host name of the user who submitted the comment.</dd>

  <dt>@user</dt>
  <dd>The name of the user who submitted the comment.</dd>

  <dt>@mail</dt>
  <dd>The e-mail address of the user who submitted the comment.</dd>

  <dt>@homepage</dt>
  <dd>The homepage of the user who submitted the comment.</dd>

  <dt>@subject</dt>
  <dd>The comment's subject line.</dd>

  <dt>@comment</dt>
  <dd>The actual comment text the user submitted.</dd>");

  $form['commentmail_mail_approve'] = array(
    '#type' => 'textarea',
    '#title' => t('Body text for approval mails'),
    '#default_value' => variable_get('commentmail_mail_approve', t(COMMENTMAIL_DEFAULT_APPROVE)),
    '#description' => $placeholders,
    '#cols' => 60,
    '#rows' => 25,
  );

  $form['commentmail_mail_notify'] = array(
    '#type' => 'textarea',
    '#title' => t('Body text for notification mails'),
    '#default_value' => variable_get('commentmail_mail_notify', t(COMMENTMAIL_DEFAULT_NOTIFY)),
    '#description' => $placeholders,
    '#cols' => 60,
    '#rows' => 25,
  );

  return system_settings_form($form);
}
