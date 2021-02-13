package com.example.demo.repo;

import org.springframework.web.reactive.function.client.WebClient;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import reactor.core.publisher.Mono;

public class TigerGraphUtil {
	private static String BASE_URL = "http://localhost:9000";
	private static WebClient WEBCLIENT = WebClient.create(BASE_URL);
	private static ObjectMapper OBJECT_MAPPER = new ObjectMapper();

	public static JsonNode post(String uri, String payload) {
		JsonNode response = null;
		try {
			response = OBJECT_MAPPER.readTree(WEBCLIENT.post().uri(uri).body(Mono.just(payload), String.class)
					.retrieve().bodyToMono(String.class).block());
		} catch (Exception e) {
			e.printStackTrace();
		}

		return response;
	}

	public static JsonNode get(String uri) {
		JsonNode response = null;
		try {
			response = OBJECT_MAPPER.readTree(WEBCLIENT.get().uri(uri).retrieve().bodyToMono(String.class).block());
		} catch (Exception e) {
			e.printStackTrace();
		}

		return response;
	}

	public static JsonNode put(String uri, String payload) {
		// TigerGraph uses POST for upserts (creates & updates)
		return post(uri, payload);
	}

	public static JsonNode delete(String uri) {
		JsonNode response = null;
		try {
			response = OBJECT_MAPPER.readTree(WEBCLIENT.delete().uri(uri).retrieve().bodyToMono(String.class).block());
		} catch (Exception e) {
			e.printStackTrace();
		}

		return response;
	}
}
