$(window).on('load', function() {
	// customizing appearance
	// *************** please, SET constants *******************************************
	const title_center = 'Администрирование учётных записей <a href="https://www.domain.org">Вашей организации</a>';
	const page_title = '<title>Администрирование учётных записей Вашей организации</title>';
	const login_domain_postfix = '<span>@domain.org</span>';
	const server_replacement_text = 'Ваша организация';
	const footer_copyright = '<div id="footer"><a href="http://www.domain.org">Ваша организация</a> &copy; 2023 - <span id="year">2020</span></div>';
	// *********************************************************************************
	$('body > table > tbody > tr > td:nth-child(2) > span a').replaceWith(title_center);
	$('body > table > tbody > tr > td:nth-child(2) > span a').css("display", "inline");
	$('head > title').replaceWith(page_title);
	if (login_domain_postfix != '') {
	    $('#username').after(login_domain_postfix);
	    $('#username').attr("style", "width: 93px!important;")
	};
	$('.lamLogo').replaceWith('<a class="lamLogo_new" href="http://www.ldap-account-manager.org/" target="new_window">Powered by LAM 7.4</a>');
	$('.lamLogo_new').css("display", "block");
	//if (logo_file_path !='') {
	//    $('.lamLogo_new').css("background-image", logo_file_path);
	//    $('.lamLogo_new').css("text-indent", "100px");
	//} else {
	    //$('.lamLogo_new').css("background-image", "url(./graphics/logo136.png)");
	//    $('.lamLogo_new').css("text-indent", "80px");
	//}
	// 05.04.2021 additional links
	//$('body > table > tbody > tr > td:nth-child(1)').after('<td align="left" height="30" class="nowrap" id="additional_links"> | <a href="http://stat.mail.ksc.ru">Лог почты</a> | <a href="http://rspamd.mail.ksc.ru">Антиспам</a> | <a href="/addressbook">Адресная книга</a></div>');
	//$('body > table > tbody > tr > td:nth-child(1)').removeAttr('width').addClass('nowrap').css('width','240px');
	
	if (server_replacement_text !='') {
	    $('body > div.centeredTable > div > table > tbody > tr:nth-child(3) > td:nth-child(1) > form > div > div:nth-child(1)').text(function(index, text) {return text.replace("LDAP сервер", server_replacement_text);});
	};
	$('body > div.centeredTable > div > table > tbody > tr:nth-child(3) > td:nth-child(1) > form > div > div:nth-child(1)').show();
	// hide RDN, userPrincipalNameDomain, email, cn and displayName attrs because they form from the principalName
	//$('#accountContainerRDN').prop( "disabled", true ); //this made troubles whel saving account settings
	$('#mail ').parent().parent().hide();
	$('#userPrincipalNameDomain').hide();
	$('#userPrincipalNameDomain').nextAll(':first').hide();
	$('#displayName').parent().parent().hide();
	// 02.04.2021 if group edit page - dont hide cn field
	//console.log($("[class*='mailAlias']"));
	if ( !($("[class*='mailAlias']").length) && !($("[class*='group']").length) ) {
			$('#cn').parent().parent().hide();
			$('#accountContainerRDN option:not(:selected)').prop('disabled', true);
			$('#accountContainerRDN').attr("style", "pointer-events: none;");
			$('#accountContainerRDN option:not(:selected)').remove();
	}
	// samoilov 09.04.2021 hide organization selector  based on profile adm_root
	if (!($( "body > table > tbody > tr > td.nowrap.header-user-label > span.hide-on-mobile > small:contains('(adm_root')" ).length)) {
		$('#o option:not(:selected)').prop('disabled', true);
		$('#o').attr("style", "pointer-events: none;");
		$('#o option:not(:selected)').remove();
		
	}
	// samoilov 07.04.2021 hide profile load button based on profile adm_root and adm_accounts
	if (!($( "body > table > tbody > tr > td.nowrap.header-user-label > span.hide-on-mobile > small:contains('(adm_root')" ).length)) {
		//if (!($( "body > table > tbody > tr > td.nowrap.header-user-label > span.hide-on-mobile > small:contains('(adm')" ).length)) {
			//$('#dropmenu > li:nth-child(3) > a').hide();
			//$('#dropmenu > li:nth-child(4)').hide();
			$('#btn_accountContainerLoadProfile').parent().hide();
		//}
	}
	// samoilov 10.09.2021 hide pdf button based on profile lists
	if ($( "body > table > tbody > tr > td.nowrap.header-user-label > span.hide-on-mobile > small:contains('(lists')" ).length) {
	    $('input[id^="btn_create"]').hide();
	}
	// 01.04.2021 add sticky table headers
	var $table = $('.accountlist');
	$table.stickyTableHeaders({
		cacheHeaderHeight: true,
	});
	// 01.04.2021 set table list settings
	if($.cookie('ListOptions_user') === null || $.cookie('ListOptions_user') === "" || $.cookie('ListOptions_user') === "null" || $.cookie('ListOptions_user') === undefined)
	{
		$.cookie.raw = true;
		$.cookie('ListOptions_user', 'L_SIZE%3D1000%3BLU_TP%3D1%3BLU_AS%3D1%3B', { expires: 365 });
	}
	if($.cookie('ListOptions_group') === null || $.cookie('ListOptions_group') === "" || $.cookie('ListOptions_group') === "null" || $.cookie('ListOptions_group') === undefined)
	{
		$.cookie.raw = true;
		$.cookie('ListOptions_group', 'L_SIZE%3D1000%3BLG_TP%3D0%3B', { expires: 365 });
	}
	if($.cookie('ListOptions_host') === null || $.cookie('ListOptions_host') === "" || $.cookie('ListOptions_host') === "null" || $.cookie('ListOptions_host') === undefined)
	{
		$.cookie.raw = true;
		$.cookie('ListOptions_host', 'L_SIZE%3D1000%3B', { expires: 365 });
	}
	// 05.04.2021 add footer copyright
	if (footer_copyright != '') {
	    $('body').append(footer_copyright);
	};
	$("#footer").css('display','block').css('text-align', 'center')
	$('#year').html(new Date().getFullYear());
})