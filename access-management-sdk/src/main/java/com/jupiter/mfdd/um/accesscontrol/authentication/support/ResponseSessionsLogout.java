package com.jupiter.mfdd.um.accesscontrol.authentication.support;

public class ResponseSessionsLogout {
    private String result;

    public String getResult() {
        return result;
    }

    public void setResult(String result) {
        this.result = result;
    }

    public boolean isSuccess() {
        return result.equals("Successfully logged out");
    }
}
