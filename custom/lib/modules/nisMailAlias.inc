<?php
/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2004 - 2019  Roland Gruber

  Customized by Yuri Samoilov 2021

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
* Provides NIS mail alias management.
*
* @package modules
* @author Roland Gruber
*/

/**
* Provides NIS mail alias management.
*
* @package modules
*/

class nisMailAlias extends baseModule {

	/** mail cache */
	private $cachedMailList = null;
	/** user cache */
	private $cachedUserList = null;
	/** display limit */
	// samoilov 18.08.2021 show textarea after 5 recipients
	const DISPLAY_LIMIT = 5;

	/**
	* Returns true if this module can manage accounts of the current type, otherwise false.
	*
	* @return boolean true if module fits
	*/
	public function can_manage() {
		return in_array($this->get_scope(), array('mailAlias'));
	}

	/**
	* Returns meta data that is interpreted by parent class
	*
	* @return array array with meta data
	*
	* @see baseModule::get_metaData()
	*/
	function get_metaData() {
		$return = array();
		// icon
		$return['icon'] = 'mailBig.png';
		// base module
		$return["is_base"] = true;
		// LDAP filter
		$return["ldap_filter"] = array('or' => "(objectClass=group)");
		//$return["ldap_filter"] = array('and' => "(!(description=service))");
		// alias name
		$return["alias"] = _("KSC RAS Mail aliases");
		// RDN attribute
		$return["RDN"] = array("cn" => "normal");
		// module dependencies
		$return['dependencies'] = array('depends' => array(), 'conflicts' => array());
		// managed object classes
		$return['objectClasses'] = array('group');
		// managed attributes
		// samoilov 18.08.2021 to allow account and alias equation comment below
		$return['attributes'] = array('cn', 'sAMAccountName', 'proxyAddresses', 'description');
		// help Entries
		$return['help'] = array(
			'alias' => array(
				"Headline" => _("Alias name"), 'attr' => 'cn',
				"Text" => _("Mails to this name are forwarded to the recipients.")
			),
			'description' => array(
				"Headline" => _("Description"), 'attr' => 'description',
				"Text" => _("Please enter a descriptive text for this user.")
			),
			'recipient' => array(
				"Headline" => _("Recipients"), 'attr' => 'proxyAddresses',
				"Text" => _("Please enter the recipients for this alias.")
			),
			'recipientList' => array(
				"Headline" => _("Recipient list"), 'attr' => 'proxyAddresses',
				"Text" => _("This is a comma separated list of recipients.")
			),
			'filter' => array(
				"Headline" => _("Filter"),
				"Text" => _("Here you can enter a filter value. Only entries which contain the filter text will be shown.")
			),
			);
		// upload fields
		$return['upload_columns'] = array(
			array(
				'name' => 'nisMailAlias_alias',
				'description' => _('Alias name'),
				'help' => 'alias',
				'example' => 'root',
				'required' => true
			),
			array(
				'name' => 'nisMailAlias_description',
				'description' => _('Description'),
				'help' => 'description',
				'example' => _('Temp, contract till December')
			),
			array(
				'name' => 'nisMailAlias_recipients',
				'description' => _('Recipient list'),
				'help' => 'recipientList',
				'example' => _('smiller')
			)
		);
		// available PDF fields
		$return['PDF_fields'] = array(
			'alias' => _('Alias name'),
			'description' => _('Description'),
			'recipients' => _('Recipient list')
		);
		return $return;
	}

	/**
	* This function fills the error message array with messages
	*/
	function load_Messages() {
		$this->messages['alias'][0] = array('ERROR', _('Alias is empty or invalid!'));  // third array value is set dynamically
		$this->messages['alias'][1] = array('ERROR', _('Account %s:') . ' nisMailAlias_alias', _('Alias is empty or invalid!'));
		$this->messages['description'][0] = array('ERROR', _('Description is empty or invalid!'));  // third array value is set dynamically
		$this->messages['description'][1] = array('ERROR', _('Account %s:') . ' nisMailAlias_description', _('Description is empty or invalid!'));
		$this->messages['recipient'][0] = array('ERROR', _('Recipient is invalid!'));  // third array value is set dynamically
		$this->messages['recipient'][1] = array('ERROR', _('Account %s:') . ' nisMailAlias_recipient', _('Recipient is invalid!'));
	}

	/**
	* Returns a list of modifications which have to be made to the LDAP account.
	*
	* @return array list of modifications
	* <br>This function returns an array with 3 entries:
	* <br>array( DN1 ('add' => array($attr), 'remove' => array($attr), 'modify' => array($attr)), DN2 .... )
	* <br>DN is the DN to change. It may be possible to change several DNs (e.g. create a new user and add him to some groups via attribute memberUid)
	* <br>"add" are attributes which have to be added to LDAP entry
	* <br>"remove" are attributes which have to be removed from LDAP entry
	* <br>"modify" are attributes which have to been modified in LDAP entry
	* <br>"info" are values with informational value (e.g. to be used later by pre/postModify actions)
	*/
	function save_attributes() {
		// skip saving if account is based on another structural object class
		if (!$this->getAccountContainer()->isNewAccount && !in_array('group', $this->getAccountContainer()->attributes_orig['objectClass'])) {
			return array();
		}
		return parent::save_attributes();
	}

	/**
	 * Returns the HTML meta data for the main account page.
	 *
	 * @return htmlElement HTML meta data
	 */
	function display_html_attributes() {
		$return = new htmlResponsiveRow();
		// alias name
		$alias = '';
		if (isset($this->attributes['cn'][0])) {
			$alias = $this->attributes['cn'][0];
		}
		$nameInput = new htmlResponsiveInputField(_('Alias name'), 'cn', $alias, 'alias');
		$nameInput->setRequired(true);
		$nameInput->setCSSClasses(array('maxwidth20'));
		$return->add($nameInput, 12);

		// samoilov 04.04.21 add description field
		$description ='';
		if (isset($this->attributes['description'][0])) {
			$description = $this->attributes['description'][0];
		}
		$descInput = new htmlResponsiveInputField(_('Description'), 'description', $description, 'description');
		$descInput->setRequired(false);
		$descInput->setCSSClasses(array('maxwidth20'));
		$return->add($descInput, 12);
		// TODO samoilov 18.08.2021 button to switch to text area
		//$return->addLabel(new htmlAccountPageButton(get_class($this), 'attributes', 'recipient', _('Список текстом')));
		
		// list current recipients
		$mailList = $this->getMailList();
		$userList = $this->getUserList();
		$autoList = array();
		if ((sizeof($userList) + sizeof($mailList)) < 300) {
			$autoList = array_merge($userList, $mailList);
		}
		$recipientCount = 0;
		if (isset($this->attributes['proxyAddresses'])) {
			natcasesort($this->attributes['proxyAddresses']);
			$this->attributes['proxyAddresses'] = array_values($this->attributes['proxyAddresses']);
			$recipientCount = sizeof($this->attributes['proxyAddresses']);
			if ($recipientCount < nisMailAlias::DISPLAY_LIMIT) {
				for ($i = 0; $i < $recipientCount; $i++) {
					if (($i == 0) && ($recipientCount == 1)) {
						$return->addLabel(new htmlOutputText(_('Recipient')));
					}
					elseif (($i == 0) && ($recipientCount > 1)) {
						$return->addLabel(new htmlOutputText(_('Recipients')));
					}
					else {
						$return->addLabel(new htmlOutputText('&nbsp;', false));
					}
					$mailField = new htmlInputField('proxyAddresses' . $i, $this->attributes['proxyAddresses'][$i]);
					if (sizeof($autoList) > 0) {
						$mailField->enableAutocompletion($autoList);
					}
					$mailField->setCSSClasses(array('maxwidth20'));
					$mailGroup = new htmlGroup();
					$mailGroup->addElement($mailField);
					$mailGroup->addElement(new htmlAccountPageButton(get_class($this), 'selectMail', 'recipient' . $i, 'mailAlias.png', true, _('Select mail')));
					//$mailGroup->addElement(new htmlAccountPageButton(get_class($this), 'selectUser', 'recipient' . $i, 'user.png', true, _('Select user')));
					$mailGroup->addElement(new htmlButton('delRec' . $i, 'del.png', true));
					$mailGroup->addElement(new htmlHelpLink('recipient'));
					$return->addField($mailGroup);
				}
			}
			else {
				$memberText = implode("\r\n", $this->attributes['proxyAddresses']);
				$return->add(new htmlResponsiveInputTextarea('proxyAddresses', $memberText, 50, 30, _('Recipients'), 'recipient'), 12);
			}
		}
		if ($recipientCount < nisMailAlias::DISPLAY_LIMIT) {
			// input box for new recipient
			$return->addLabel(new htmlOutputText(_('New recipient')));
			$newMailField = new htmlInputField('proxyAddresses');
			$newMailField->setOnKeyPress('SubmitForm(\'addRec\', event);');
			if (sizeof($autoList) > 0) {
				$newMailField->enableAutocompletion($autoList);
			}
			$newMailField->setCSSClasses(array('maxwidth20'));
			$newGroup = new htmlGroup();
			$newGroup->addElement($newMailField);
			$newGroup->addElement(new htmlAccountPageButton(get_class($this), 'selectMail', 'recipient' . 'New', 'mailAlias.png', true, _('Select mail')));
			//$newGroup->addElement(new htmlAccountPageButton(get_class($this), 'selectUser', 'recipient' . 'New', 'user.png', true, _('Select user')));
			$newGroup->addElement(new htmlButton('addRec', 'add.png', true));
			$newGroup->addElement(new htmlHelpLink('recipient'));
			$newGroup->addElement(new htmlHiddenInput('rec_number', $recipientCount));
			$return->addField($newGroup);
		}
		return $return;
	}

	/**
	* Processes user input of the primary module page.
	* It checks if all input values are correct and updates the associated LDAP attributes.
	*
	* @return array list of info/error messages
	*/
	function process_attributes() {
		$errors = array();
		$this->attributes['cn'] = array();
		$this->attributes['description'] = array();
		$this->attributes['sAMAccountName'] = array();
		$recipientCount = !empty($this->attributes['proxyAddresses']) ? sizeof($this->attributes['proxyAddresses']) : 0;
		$this->attributes['proxyAddresses'] = array();
		// check alias name
		// samoilov 10.09.2021 check alias name for no unallowed symbols and letters (russian for example)
		if (isset($_POST['cn']) && ($_POST['cn'] != "") && get_preg($_POST['cn'], 'nis_alias') && preg_match("/^[^\W_][a-zA-Z0-9._\-]+$/",$_POST['cn'])) {
			$this->attributes['cn'][] = $_POST['cn'];
		}
		else {
				$this->attributes['cn'][] = $_POST['cn'];
				$message = $this->messages['alias'][0];
				$message[] = $_POST['cn'];
				$errors[] = $message;
		}
		// samoilov 10.08.2021 process sAMAccountName
		// samoilov 18.08.2021 to allow account and alias equation comment below
		// commented out for now...
		//echo "CN: ".$this->attributes['cn'][0]."\n";
		//echo "sAMAcc: ".$this->attributes['sAMAccountName'][0];
		//echo "sAMAcc: ".$_POST['sAMAccountName'];
		if ($this->attributes['sAMAccountName'][0]!="" || $this->attributes['sAMAccountName'][0]!=$this->attributes['cn'][0]) {
		    $this->attributes['sAMAccountName'][0] = $this->attributes['cn'][0];
		}
		// samoilov 04.04.2021 process description
		if (isset($_POST['description']) && ($_POST['description'] != "")) {
			$this->attributes['description'][0] = $_POST['description'];
		}
		if ($recipientCount < nisMailAlias::DISPLAY_LIMIT) {
			// check old recipients
			if (isset($_POST['rec_number'])) {
				for ($i = 0; $i < $_POST['rec_number']; $i++) {
					if (isset($_POST['delRec' . $i])) continue;
					if (isset($_POST['proxyAddresses' . $i]) && ($_POST['proxyAddresses' . $i] != "")) {
						// check if address has correct format
						if (!get_preg($_POST['proxyAddresses' . $i], 'nis_recipient') && !get_preg($_POST['proxyAddresses' . $i], 'email')) {
							$message = $this->messages['recipient'][0];
							$message[] = $_POST['proxyAddresses' . $i];
							$errors[] = $message;
						}
						$this->attributes['proxyAddresses'][] = $_POST['proxyAddresses' . $i];
					}
				}
			}
			// check new recipient
			if (isset($_POST['proxyAddresses']) && ($_POST['proxyAddresses'] != "")) {
				// check if recipient has correct format
				if (get_preg($_POST['proxyAddresses'], 'nis_recipient') || get_preg($_POST['proxyAddresses'], 'email')) {
					$this->attributes['proxyAddresses'][] = $_POST['proxyAddresses'];
				}
				else {
						$message = $this->messages['recipient'][0];
						$message[] = $_POST['proxyAddresses'];
						$errors[] = $message;
				}
			}
		}
		else {
			$recipients = explode("\r\n", $_POST['proxyAddresses']);
			foreach ($recipients as $recipient) {
				$recipient = trim($recipient);
				if (empty($recipient)) {
					continue;
				}
				$this->attributes['proxyAddresses'][] = $recipient;
				// check if recipient has correct format
				if (!get_preg($recipient, 'nis_recipient') && !get_preg($recipient, 'email')) {
					$message = $this->messages['recipient'][0];
					$message[] = htmlspecialchars($recipient);
					$errors[] = $message;
				}
			}
		}
		$this->attributes['proxyAddresses'] = array_unique($this->attributes['proxyAddresses']);
		//console_log(print_r($this));
		return $errors;
	}

	/**
	* Displays the mail selection.
	*
	* @return htmlElement meta HTML code
	*/
	function display_html_selectMail() {
		return $this->display_html_select(true);
	}

	/**
	* Processes user input of the host/user selection page.
	* It checks if all input values are correct and updates the associated LDAP attributes.
	*
	* @return array list of info/error messages
	*/
	function process_selectMail() {
		return $this->process_select();
	}

	/**
	* Displays the user selection.
	*
	* @return htmlElement meta HTML code
	*/
	function display_html_selectUser() {
		return $this->display_html_select(false);
	}

	/**
	* Processes user input of the host/user selection page.
	* It checks if all input values are correct and updates the associated LDAP attributes.
	*
	* @return array list of info/error messages
	*/
	function process_selectUser() {
		return $this->process_select();
	}

	/**
	* Displays the user/mail selection.
	*
	* @param boolean $isMail mail selection (user selection if false)
	* @return htmlElement meta HTML code
	*/
	function display_html_select($isMail) {
		$options = array();
		if ($isMail) {
			$regex = 'email';
			$options = $this->getMailList();
			$suffix = 'Mail';
			$label = _('Email');
		}
		else {
			$regex = 'username';
			$options = $this->getUserList();
			$suffix = 'User';
			$label = _('User');
		}
		$return = new htmlResponsiveRow();
		$postKeys = array_keys($_POST);
		$position = 'New';
		for ($i = 0; $i < sizeof($postKeys); $i++) {
			if 	(strpos($postKeys[$i], 'form_subpage_' . get_class($this) . '_select' . $suffix . '_recipient') === 0) {
				$position = substr($postKeys[$i], strlen('form_subpage_' . get_class($this) . '_select' . $suffix . '_recipient'));
				break;
			}
		}
		// load list with all mail addresses
		$count = sizeof($options);
		for ($i = 0; $i < $count; $i++) {
			if (!get_preg($options[$i], $regex)) {
				unset($options[$i]);
			}
		}
		$options = array_values($options);
		$return->addLabel(new htmlOutputText(_('Filter')));
		$filterGroup = new htmlGroup();
		$filterInput = new htmlInputField('filter', null);
		$filterInput->setCSSClasses(array('maxwidth10'));
		$filterInput->filterSelectBox('selectBox');
		$filterGroup->addElement($filterInput);
		$filterGroup->addElement(new htmlHelpLink('filter'));
		$return->addField($filterGroup);
		$return->addLabel(new htmlOutputText($label));
		$mailSelect = new htmlSelect('selectBox', $options, array(), 15);
		$mailSelect->setMultiSelect($position === 'New');
		$mailSelect->colspan = 5;
		$return->addField($mailSelect);
		$return->addVerticalSpacer('2rem');
		$return->addLabel(new htmlAccountPageButton(get_class($this), 'attributes', 'select', _('Ok')));
		$return->addField(new htmlAccountPageButton(get_class($this), 'attributes', 'back', _('Cancel')));
		$return->add(new htmlHiddenInput('position', $position), 12);
		return $return;
	}

	/**
	* Processes user input of the host/user selection page.
	* It checks if all input values are correct and updates the associated LDAP attributes.
	*
	* @return array list of info/error messages
	*/
	function process_select() {
		if (isset($_POST['form_subpage_' . get_class($this) . '_attributes_select'])) {
			if (!isset($this->attributes['proxyAddresses'])) {
				$this->attributes['proxyAddresses'] = array();
			}
			$position = $_POST['position'];
			if ($position == 'New') {
				$this->attributes['proxyAddresses'] = array_merge($this->attributes['proxyAddresses'], $_POST['selectBox']);
			}
			else {
				$this->attributes['proxyAddresses'][$_POST['position']] = $_POST['selectBox'];
			}
			return array();
		}
		return array();
	}

	/**
	* Controls if the module button the account page is visible and activated.
	*
	* @return string status ("enabled", "disabled", "hidden")
	*/
	function getButtonStatus() {
		if (!$this->getAccountContainer()->isNewAccount) {
			// check if account is based on our object class
			$objectClasses = $this->getAccountContainer()->attributes_orig['objectClass'];
			if (is_array($objectClasses) && !in_array('group', $objectClasses)) {
				return "disabled";
			}
		}
		return "enabled";
	}

	/**
	 * {@inheritDoc}
	 * @see baseModule::build_uploadAccounts()
	 */
	function build_uploadAccounts($rawAccounts, $ids, &$partialAccounts, $selectedModules, &$type) {
		$messages = array();
		for ($i = 0; $i < sizeof($rawAccounts); $i++) {
			// add object class
			if (!in_array("group", $partialAccounts[$i]['objectClass'])) $partialAccounts[$i]['objectClass'][] = "group";
			// add alias name
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'nisMailAlias_alias', 'cn',
				'nis_alias', $this->messages['alias'][1], $messages);
			// samoilov 04.04.21 add desciption 
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'nisMailAlias_description', 'description',
				'nis_alias', $this->messages['description'][1], $messages);
			// add recipients
			if ($rawAccounts[$i][$ids['nisMailAlias_recipients']] != "") {
				$aliases = explode(',', $rawAccounts[$i][$ids['nisMailAlias_recipients']]);
				// check format
				for ($a = 0; $a < sizeof($aliases); $a++) {
					if (get_preg($aliases[$a], 'nis_recipient') || get_preg($aliases[$a], 'email')) {
						$partialAccounts[$i]['proxyAddresses'][] = $aliases[$a];
					}
					else {
						$errMsg = $this->messages['recipient'][1];
						array_push($errMsg, array($i));
						$messages[] = $errMsg;
					}
				}
			}
		}
		return $messages;
	}

	/**
	 * {@inheritDoc}
	 * @see baseModule::get_pdfEntries()
	 */
	function get_pdfEntries($pdfKeys, $typeId) {
		$return = array();
		$this->addSimplePDFField($return, 'alias', _('Alias name'), 'cn');
		$this->addSimplePDFField($return, 'recipients', _('Recipient list'), 'proxyAddresses');
		return $return;
	}

	/**
	 * Returns a list of existing email addresses.
	 *
	 * @return array email addresses
	 */
	private function getMailList() {
		if ($this->cachedMailList != null) {
			return $this->cachedMailList;
		}
		//$this->cachedMailList = searchLDAPByAttribute('mail', '*', null, array('mail'), array('user'));
		//samoilov 10.09.2021 add description attr filter of emails list
		$this->cachedMailList = searchLDAPByFilter('(&(!(description=service))(objectClass=person)(mail=*))', array('mail'), array('user'));
		for ($i = 0; $i < sizeof($this->cachedMailList); $i++) {
			$this->cachedMailList[$i] = $this->cachedMailList[$i]['mail'][0];
		}
		$this->cachedMailList = array_values(array_unique($this->cachedMailList));
		return $this->cachedMailList;
	}

	/**
	 * Returns a list of existing user names.
	 *
	 * @return array user names
	 */
	private function getUserList() {
		if ($this->cachedUserList != null) {
			return $this->cachedUserList;
		}
		//$this->cachedUserList = searchLDAPByFilter('(|(objectClass=posixAccount)(objectClass=inetOrgPerson))', array('uid'), array('user'));
		// samoilov 10.08.2021
		$this->cachedUserList = searchLDAPByFilter('(&(|(objectClass=person)(objectClass=user)))', array('uid'), array('user'));
		$count = sizeof($this->cachedUserList);
		for ($i = 0; $i < $count; $i++) {
			if (empty($this->cachedUserList[$i]['uid'][0])) {
				unset($this->cachedUserList[$i]);
				continue;
			}
			$this->cachedUserList[$i] = $this->cachedUserList[$i]['uid'][0];
		}
		$this->cachedUserList = array_values(array_unique($this->cachedUserList));
		return $this->cachedUserList;
	}

}


?>
