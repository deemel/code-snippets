package com.jupiter.adp.log.rest.api;

public enum SourceSyntaxEnum {
    Unspecified, _3gpp_dn, Xpath, Uuid, Oid, File, Class;

    public String toString() {
        return ((this.name().startsWith("_")) ? (this.name().substring(1))
                : (this.name()));
    }

    public static SourceSyntaxEnum valueOf2(String string) {
        return ((string.equals(_3gpp_dn.toString())) ? (_3gpp_dn)
                : (valueOf(string)));
    }
}
