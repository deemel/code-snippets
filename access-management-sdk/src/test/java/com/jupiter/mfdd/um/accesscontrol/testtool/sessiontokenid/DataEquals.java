package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontokenid;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.SessionTokenIDImpl;
import com.jupiter.mfdd.um.accesscontrol.support.Realm;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataEquals implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        test(sessionTokenId, sessionTokenId);
        test(sessionTokenId, new Integer(7));

        Realm realm = null;
        try {
            realm = new Realm("/realm");
        } catch (AuthXException e) {
            e.printStackTrace();
        }

        try {
            test(sessionTokenId,
                    new SessionTokenIDImpl(sessionTokenId.getSessionTokenId(),
                            realm, "userId", "password"));
        } catch (AuthXException e) {
            e.printStackTrace();
        }

        try {
            test(sessionTokenId, new SessionTokenIDImpl("sessionTokenIdString",
                    realm, "userId", "password"));
        } catch (AuthXException e) {
            e.printStackTrace();
        }
    }

    private void test(SessionTokenID sessionTokenId, Object object) {
        try {
            System.out.println();
            System.out.println("equals(" + object + "):");

            boolean rv = sessionTokenId.equals(object);

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
