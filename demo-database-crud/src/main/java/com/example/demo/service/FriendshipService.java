package com.example.demo.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Service;

import com.example.demo.model.FriendshipModel;
import com.example.demo.repo.FriendshipRepo;
import com.example.demo.repo.FriendshipRepoTigerGraph;

@Service
public class FriendshipService {
	private final FriendshipRepo friendshipRepo;

	@Autowired
//	public FriendshipService(@Qualifier(FriendshipRepoInMemory.NAME) FriendshipRepo friendshipRepo) {
	public FriendshipService(@Qualifier(FriendshipRepoTigerGraph.NAME) FriendshipRepo friendshipRepo) {
		this.friendshipRepo = friendshipRepo;
	}

	public Object post(FriendshipModel friendship) {
		return friendshipRepo.post(friendship);
	}

	public Object get() {
		return friendshipRepo.get();
	}

	public Object get(String name) {
		return friendshipRepo.get(name);
	}

	public Object get(String nameFrom, String nameTo) {
		return friendshipRepo.get(nameFrom, nameTo);
	}

	public Object put(FriendshipModel friendship) {
		return friendshipRepo.put(friendship);
	}

	public Object delete(String nameFrom, String nameTo) {
		return friendshipRepo.delete(nameFrom, nameTo);
	}
}
