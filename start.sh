#!/bin/bash
set -e

WEB_USER="www-data"

cp -f /etc/ldap-account-manager/config/*.sample /usr/share/ldap-account-manager/config/ 2>/dev/null || true
[ -f /etc/ldap-account-manager/config/language ] && cp -f /etc/ldap-account-manager/config/language /usr/share/ldap-account-manager/config/language
[ -f /etc/ldap-account-manager/config/.htaccess ] && cp -f /etc/ldap-account-manager/config/.htaccess /usr/share/ldap-account-manager/config/.htaccess
[ -d /etc/ldap-account-manager/config/pdf ] && cp -rf /etc/ldap-account-manager/config/pdf /usr/share/ldap-account-manager/config/
[ -d /etc/ldap-account-manager/config/profiles ] && cp -rf /etc/ldap-account-manager/config/profiles /usr/share/ldap-account-manager/config/
[ -d /etc/ldap-account-manager/config/selfService ] && cp -rf /etc/ldap-account-manager/config/selfService /usr/share/ldap-account-manager/config/
[ -d /etc/ldap-account-manager/config/templates ] && cp -rf /etc/ldap-account-manager/config/templates /usr/share/ldap-account-manager/config/

if [ ! -f /usr/share/ldap-account-manager/config/config.cfg ] && [ ! -f /etc/ldap-account-manager/config.cfg ]; then
    if [ -f /etc/ldap-account-manager/config.cfg.sample ]; then
        cp /etc/ldap-account-manager/config.cfg.sample /usr/share/ldap-account-manager/config/config.cfg
    fi
fi

mkdir -p /usr/share/ldap-account-manager/sess /usr/share/ldap-account-manager/tmp/internal
chown -R ${WEB_USER}:${WEB_USER} /usr/share/ldap-account-manager/sess
chown -R ${WEB_USER}:${WEB_USER} /usr/share/ldap-account-manager/tmp
chown -R ${WEB_USER}:${WEB_USER} /usr/share/ldap-account-manager/config
[ -f /etc/ldap-account-manager/config.cfg ] && chown ${WEB_USER}:${WEB_USER} /etc/ldap-account-manager/config.cfg

if [ -n "$LAM_NGINX_PATH" ] && [ -f /etc/ldap-account-manager/nginx.conf ]; then
    sed -i "s|location /lam|location /${LAM_NGINX_PATH}|g" /etc/ldap-account-manager/nginx.conf
    sed -i 's/root/alias/g' /etc/ldap-account-manager/nginx.conf
fi

NGINX_DEFAULT_CONF="/etc/nginx/sites-enabled/default.conf"

if [ -n "$LAM_LISTEN_PORT" ] && [ -f "$NGINX_DEFAULT_CONF" ]; then
    sed -i "s/listen 8080/listen ${LAM_LISTEN_PORT}/g" "$NGINX_DEFAULT_CONF"
    sed -i "s/listen \[::\]:8080/listen [::]:${LAM_LISTEN_PORT}/g" "$NGINX_DEFAULT_CONF"
fi

exec supervisord -n -c /etc/supervisor/supervisord.conf
