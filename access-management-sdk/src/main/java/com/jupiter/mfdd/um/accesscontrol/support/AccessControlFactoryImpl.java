package com.jupiter.mfdd.um.accesscontrol.support;

import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationService;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationService;
import com.jupiter.mfdd.um.accesscontrol.authentication.AuthenticationServiceImpl;
import com.jupiter.mfdd.um.accesscontrol.authorization.AuthorizationServiceImpl;

public class AccessControlFactoryImpl implements AccessControlFactory {
    @Override
    public AuthenticationService getAuthenticationService() {
        return new AuthenticationServiceImpl();
    }

    @Override
    public AuthorizationService getAuthorizationService() {
        return new AuthorizationServiceImpl();
    }
}
