package com.jupiter.mfdd.um.accesscontrol.authentication.support;

public class ResponseAuthenticate {
    private String tokenId;
    private String successUrl;

    public String getTokenId() {
        return tokenId;
    }

    public String getSuccessUrl() {
        return successUrl;
    }

    public void setTokenId(String tokenId) {
        this.tokenId = tokenId;
    }

    public void setSuccessUrl(String successUrl) {
        this.successUrl = successUrl;
    }
}
