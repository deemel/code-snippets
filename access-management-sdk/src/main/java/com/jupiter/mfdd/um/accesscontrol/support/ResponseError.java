package com.jupiter.mfdd.um.accesscontrol.support;

import javax.ws.rs.core.Response.Status;

public class ResponseError {
    private Status status;
    private int code;
    private String reason;
    private String message;

    public ResponseError() {
    }

    public ResponseError(int code, String message) {
        this.message = message;

        setCode(code);
        if (status != null)
            reason = status.getReasonPhrase();
    }

    public int getCode() {
        return code;
    }

    public String getReason() {
        return reason;
    }

    public String getMessage() {
        return message;
    }

    public void setCode(int code) {
        this.code = code;
        status = Status.fromStatusCode(this.code);
    }

    public void setReason(String reason) {
        this.reason = reason;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String toString() {
        return "status <" + status + ">, code <" + code + ">, reason <"
                + reason + ">, message <" + message + ">";
    }
}
