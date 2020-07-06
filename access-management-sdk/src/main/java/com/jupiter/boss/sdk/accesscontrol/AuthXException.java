package com.jupiter.boss.sdk.accesscontrol;

/**
 * Authentication Exception
 * 
 * @version 1.0.0
 *
 */
public class AuthXException extends Exception {
    private static final long serialVersionUID = 1L;

    public AuthXException(String msg) {
        super(msg);
    };

    public AuthXException(String msg, Throwable t) {
        super(msg, t);
    };

    public AuthXException(Throwable t) {
        super(t);
    }
}
