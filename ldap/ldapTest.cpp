#include <stdio.h>
#include <ldap.h>
#include <string.h>

int main()
{
    const char *ldapUri = "ldap://ldap.technikum-wien.at:389";

    // Benutzer auffordern, die UserID und das Passwort einzugeben
    char user[256];
    char password[256];

    printf("Geben Sie die UserID ein: ");
    fgets(user, sizeof(user), stdin);
    user[strcspn(user, "\n")] = 0; // Entfernen des Zeilenumbruchs

    printf("Geben Sie das Passwort ein: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Entfernen des Zeilenumbruchs

    // Der vollständige Distinguished Name (DN) für den Benutzer
    char dn[512];
    snprintf(dn, sizeof(dn), "uid=%s,ou=people,dc=technikum-wien,dc=at", user);

    LDAP *ldapHandle;
    int rc = ldap_initialize(&ldapHandle, ldapUri);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Verbinden zum LDAP-Server: %s\n", ldap_err2string(rc));
        return 1;
    }
    printf("Mit dem LDAP-Server verbunden\n");

    // LDAP-Protokollversion setzen
    int ldapVersion = LDAP_VERSION3;
    rc = ldap_set_option(ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion);
    if (rc != LDAP_OPT_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Setzen der LDAP-Version: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return 1;
    }

    // TLS-Verbindung starten
    rc = ldap_start_tls_s(ldapHandle, NULL, NULL);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "Fehler beim Starten von TLS: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return 1;
    }
    printf("TLS-Verbindung hergestellt\n");

    // Perform bind
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

    printf("Bind erfolgreich\n");

    ldap_unbind_ext_s(ldapHandle, NULL, NULL);
    return 0;
}
