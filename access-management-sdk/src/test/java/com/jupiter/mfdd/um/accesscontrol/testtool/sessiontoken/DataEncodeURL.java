package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataEncodeURL implements CallMethod {
    private String url;

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("encodeURL(" + url + "):");

            String rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().getSessionToken(sessionTokenId)
                    .encodeURL(url);

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
