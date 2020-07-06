package com.jupiter.mfdd.um.accesscontrol.testtool;

import org.apache.commons.lang3.StringUtils;
import org.kohsuke.args4j.CmdLineParser;
import org.kohsuke.args4j.ParserProperties;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.SessionTokenIDImpl;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;
import com.jupiter.mfdd.um.accesscontrol.support.Realm;

public class Main {
    private static final String CLASS_NAME = Main.class.getName();
    private static final String STR_DUMMY = "dummy";

    public static void main(String[] args) {
        Arguments arguments = new Arguments();
        CmdLineParser parser = new CmdLineParser(arguments, ParserProperties
                .defaults().withUsageWidth(640).withOptionSorter(null));

        try {
            parser.parseArgument(args);
            arguments.parseOptions();
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
            System.out.println();
            printUsage();

            return;
        }

        if (arguments.help) {
            printUsage();

            return;
        }

        System.out
                .println("============================================================");
        System.out.println("UM server URL <" + arguments.umServerUrl + ">");
        System.out.println("Session token ID string <"
                + arguments.sessionTokenIdString + ">");
        System.out.println("Realm <" + arguments.realm + ">");
        System.out
                .println("============================================================");

        System.setProperty(OpenAmRestUtil.PROP_UM_SERVER_URL,
                arguments.umServerUrl);

        SessionTokenID sessionTokenId = null;

        if (arguments.authenticate != null) {
            /*
             * AuthenticationService interface's authenticate
             */
            System.out.println();
            System.out
                    .println("============================================================");
            System.out
                    .println("AuthenticationService interface's authenticate");
            System.out
                    .println("============================================================");

            arguments.dataAuthenticate.callMethod(null);

            sessionTokenId = arguments.dataAuthenticate.sessionTokenId;
        } else {
            if (StringUtils.isEmpty(arguments.sessionTokenIdString)) {
                System.out.println();
                System.out.println("Invalid session token ID string <"
                        + arguments.sessionTokenIdString + ">; exiting.");
                return;
            }

            if (StringUtils.isEmpty(arguments.realm)) {
                System.out.println();
                System.out.println("Invalid realm <" + arguments.realm
                        + ">; exiting.");
                return;
            }

            try {
                sessionTokenId = new SessionTokenIDImpl(
                        arguments.sessionTokenIdString, new Realm(
                                arguments.realm), STR_DUMMY, STR_DUMMY);
            } catch (AuthXException e) {
                System.out.println();
                System.out.println("Error: " + e.getMessage());
                System.out.println("Exiting.");
                return;
            }
        }

        try {
            OpenAmRestUtil.validateSessionTokenId(sessionTokenId);
        } catch (AuthXException e) {
            System.out.println();
            System.out.println("Invalid session token ID <" + sessionTokenId
                    + ">; exiting.");
            return;
        }

        /*
         * SessionTokenID interface
         */
        System.out.println();
        System.out
                .println("============================================================");
        System.out.println("SessionTokenID interface");
        System.out
                .println("============================================================");

        if (arguments.dataGetSessionTokenId != null)
            arguments.dataGetSessionTokenId.callMethod(sessionTokenId);

        if (arguments.dataEquals != null)
            arguments.dataEquals.callMethod(sessionTokenId);

        /*
         * SessionToken interface
         */
        System.out.println();
        System.out
                .println("============================================================");
        System.out.println("SessionToken interface");
        System.out
                .println("============================================================");

        if (arguments.dataGetSessionTokenID != null)
            arguments.dataGetSessionTokenID.callMethod(sessionTokenId);

        if (arguments.dataGetMaxSessionTime != null)
            arguments.dataGetMaxSessionTime.callMethod(sessionTokenId);

        if (arguments.dataGetIdleTime != null)
            arguments.dataGetIdleTime.callMethod(sessionTokenId);

        if (arguments.dataGetTimeLeft != null)
            arguments.dataGetTimeLeft.callMethod(sessionTokenId);

        if (arguments.dataGetMaxIdleTime != null)
            arguments.dataGetMaxIdleTime.callMethod(sessionTokenId);

        if (arguments.dataSetProperty != null)
            arguments.dataSetProperty.callMethod(sessionTokenId);

        if (arguments.dataGetProperty != null)
            arguments.dataGetProperty.callMethod(sessionTokenId);

        if (arguments.dataGetPropertyLong != null)
            arguments.dataGetPropertyLong.callMethod(sessionTokenId);

        if (arguments.dataIsTokenRestricted != null)
            arguments.dataIsTokenRestricted.callMethod(sessionTokenId);

        if (arguments.dataEncodeURL != null)
            arguments.dataEncodeURL.callMethod(sessionTokenId);

        if (arguments.dataGetAuthLevel != null)
            arguments.dataGetAuthLevel.callMethod(sessionTokenId);

        if (arguments.dataGetAuthType != null)
            arguments.dataGetAuthType.callMethod(sessionTokenId);

        if (arguments.dataGetIPAddress != null)
            arguments.dataGetIPAddress.callMethod(sessionTokenId);

        if (arguments.dataGetHostName != null)
            arguments.dataGetHostName.callMethod(sessionTokenId);

        if (arguments.dataGetPrincipal != null)
            arguments.dataGetPrincipal.callMethod(sessionTokenId);

        /*
         * AuthenticationService interface
         */
        System.out.println();
        System.out
                .println("============================================================");
        System.out.println("AuthenticationService interface");
        System.out
                .println("============================================================");

        if (arguments.dataGetSessionToken != null)
            arguments.dataGetSessionToken.callMethod(sessionTokenId);

        if (arguments.dataRefreshSession != null)
            arguments.dataRefreshSession.callMethod(sessionTokenId);

        if (arguments.dataIsValidSession != null)
            arguments.dataIsValidSession.callMethod(sessionTokenId);

        /*
         * AuthorizationService interface
         */
        System.out.println();
        System.out
                .println("============================================================");
        System.out.println("AuthorizationService interface");
        System.out
                .println("============================================================");

        if (arguments.dataIsAuthorize != null)
            arguments.dataIsAuthorize.callMethod(sessionTokenId);

        if (arguments.dataIsAuthorizeLong != null)
            arguments.dataIsAuthorizeLong.callMethod(sessionTokenId);

        if (arguments.dataAuthorize != null)
            arguments.dataAuthorize.callMethod(sessionTokenId);

        if (arguments.dataAuthorizeLong != null)
            arguments.dataAuthorizeLong.callMethod(sessionTokenId);

        if (arguments.dataDestroySession == null)
            return;

        /*
         * AuthenticationService interface's destroySession
         */
        System.out.println();
        System.out
                .println("============================================================");
        System.out.println("AuthenticationService interface's destroySession");
        System.out
                .println("============================================================");

        arguments.dataDestroySession.callMethod(sessionTokenId);
    }

    public static void printUsage() {
        System.out.println("USAGE: java -cp <classpath> " + CLASS_NAME
                + " [options...]");
        System.out.println("Options:");
        new CmdLineParser(new Arguments(), ParserProperties.defaults()
                .withUsageWidth(640).withOptionSorter(null))
                .printUsage(System.out);
    }
}
