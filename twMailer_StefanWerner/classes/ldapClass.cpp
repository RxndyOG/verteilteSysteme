#include "ldapClass.h"

#include <stdio.h>
#include <ldap.h>
#include <string.h>
#include <iostream>

ldapClass::ldapClass()
{

}

ldapClass::~ldapClass()
{

}


int ldapClass::connectToLDAP(std::string username, std::string pwd)
{
    const char *ldapUri = "ldap://ldap.technikum-wien.at:389";

    char user[256];
    char password[256];

    strcpy(user, username.c_str());  
    strcpy(password, pwd.c_str()); 

    char dn[512];
    snprintf(dn, sizeof(dn), "uid=%s,ou=people,dc=technikum-wien,dc=at", user);

    LDAP *ldapHandle;
    int rc = ldap_initialize(&ldapHandle, ldapUri);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Verbinden zum LDAP-Server: %s\n", ldap_err2string(rc));
        return 1;
    }

    int ldapVersion = LDAP_VERSION3;
    rc = ldap_set_option(ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion);
    if (rc != LDAP_OPT_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Setzen der LDAP-Version: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return 1;
    }

    rc = ldap_start_tls_s(ldapHandle, NULL, NULL);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Starten von TLS: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return 1;
    }

    BerValue credentials;
    credentials.bv_val = password;
    credentials.bv_len = strlen(password);

    rc = ldap_sasl_bind_s(ldapHandle, dn, LDAP_SASL_SIMPLE, &credentials, NULL, NULL, NULL);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "Bind fehlgeschlagen: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return 1;
    }

    ldap_unbind_ext_s(ldapHandle, NULL, NULL);
    return 0;
}
