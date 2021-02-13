package com.example.demo.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.model.FriendshipModel;
import com.example.demo.service.FriendshipService;

@RestController
@RequestMapping("api/v1/friendship")
public class FriendshipController {
	private final FriendshipService friendshipService;

	@Autowired
	public FriendshipController(FriendshipService friendshipService) {
		this.friendshipService = friendshipService;
	}

	@PostMapping
	public Object post(@RequestBody FriendshipModel friendship) {
		return friendshipService.post(friendship);
	}

	@GetMapping
	public Object get(@RequestParam(name = "nameFrom", required = false) String nameFrom,
			@RequestParam(name = "nameTo", required = false) String nameTo) {
		if ((nameFrom == null) && (nameTo == null))
			return friendshipService.get();

		if ((nameFrom == null) && (nameTo != null))
			return friendshipService.get(nameTo);

		if ((nameFrom != null) && (nameTo == null))
			return friendshipService.get(nameFrom);

		return friendshipService.get(nameFrom, nameTo);
	}

	@PutMapping
	public Object put(@RequestBody FriendshipModel friendship) {
		return friendshipService.put(friendship);
	}

	@DeleteMapping
	public Object delete(@RequestParam("nameFrom") String nameFrom, @RequestParam("nameTo") String nameTo) {
		return friendshipService.delete(nameFrom, nameTo);
	}
}
