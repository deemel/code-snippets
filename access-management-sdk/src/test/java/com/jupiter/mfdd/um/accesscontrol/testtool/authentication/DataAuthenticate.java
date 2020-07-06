package com.jupiter.mfdd.um.accesscontrol.testtool.authentication;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationResponse;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataAuthenticate implements CallMethod {
    public SessionTokenID sessionTokenId;

    private String realm;
    private String userId;
    private String password;

    public String getRealm() {
        return realm;
    }

    public String getUserId() {
        return userId;
    }

    public String getPassword() {
        return password;
    }

    public void setRealm(String realm) {
        this.realm = realm;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("authenticate(" + realm + ", " + userId + ", "
                    + password + "):");

            AuthenticationResponse rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().authenticate(realm, userId,
                            password);

            System.out.println("rv: responseCode <" + rv.getResponseCode()
                    + ">, sessionTokenId <" + rv.getSessionTokenID() + ">");

            this.sessionTokenId = rv.getSessionTokenID();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
