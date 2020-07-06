package com.jupiter.mfdd.um.accesscontrol.support;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationService;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationService;

public interface AccessControlFactory {
    public AuthenticationService getAuthenticationService()
            throws AuthXException;

    public AuthorizationService getAuthorizationService() throws AuthXException;
}
