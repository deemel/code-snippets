package com.example.demo;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;

import org.apache.commons.codec.digest.DigestUtils;

import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.Session;

public class Main {
	private static final String HOST = System.getenv("HOST");
	private static final String USER = System.getenv("USER");
	private static final String PASSWD = System.getenv("PASSWD");
	private static final String FILENAME_KNOWN_HOSTS = "/" + USER + "/.ssh/known_hosts";
	private static final String FILENAME = "/tmp/demo-sftp.txt";
	private static final String FILENAME_GET = FILENAME + ".get";
	private static final String FILENAME_PUT = FILENAME + ".put";
	private static final String FILE_CONTENT = "This file is " + FILENAME + ".\n";

	public static void main(String[] args) throws Exception {
		System.out.println("Connecting to " + HOST + ".");

		JSch jsch = new JSch();
		jsch.setKnownHosts(FILENAME_KNOWN_HOSTS);

		Session session = jsch.getSession(USER, HOST);
		session.setPassword(PASSWD);
//		session.setConfig("StrictHostKeyChecking", "no");
		session.connect();

		ChannelSftp channel = (ChannelSftp) session.openChannel("sftp");
		channel.connect();

		System.out.println("Creating " + FILENAME + ".");
		Files.writeString(Paths.get(FILENAME), FILE_CONTENT);

		System.out.println("Testing SFTP get " + FILENAME + ".");
		channel.get(FILENAME, FILENAME_GET);

		System.out.println("Testing SFTP put " + FILENAME + ".");
		channel.put(FILENAME, FILENAME_PUT);

		System.out.println("Disconnecting from " + HOST + ".");
		channel.disconnect();
		channel.exit();

		session.disconnect();

		String checksum0 = null;
		boolean isEqual = false;
		for (String i : Arrays.asList(FILENAME, FILENAME_GET, FILENAME_PUT)) {
			String checksum = DigestUtils.md5Hex(Files.readAllBytes(Paths.get(i)));
			System.out.println("Checksum " + i + ": " + checksum);

			if (checksum0 == null) {
				checksum0 = checksum;
				continue;
			}

			isEqual = checksum.equals(checksum0);
		}

		System.out.println("Checksums equal? " + isEqual);
	}
}
