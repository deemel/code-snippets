package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontokenid;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetSessionTokenId implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getSessionTokenId():");

            String rv = sessionTokenId.getSessionTokenId();

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
