package com.jupiter.mfdd.um.accesscontrol.authentication.support;

public class ResponseSessionsValidate {
    private boolean valid;
    private String uid;
    private String realm;

    public boolean getValid() {
        return valid;
    }

    public String getUid() {
        return uid;
    }

    public String getRealm() {
        return realm;
    }

    public void setValid(boolean valid) {
        this.valid = valid;
    }

    public void setUid(String uid) {
        this.uid = uid;
    }

    public void setRealm(String realm) {
        this.realm = realm;
    }
}
