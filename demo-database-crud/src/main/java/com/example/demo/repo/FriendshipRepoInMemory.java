package com.example.demo.repo;

import java.util.LinkedList;
import java.util.List;

import org.springframework.stereotype.Repository;

import com.example.demo.model.FriendshipModel;

@Repository(FriendshipRepoInMemory.NAME)
public class FriendshipRepoInMemory implements FriendshipRepo {
	public static final String NAME = "friendshipRepoInMemory";

	private static List<FriendshipModel> REPO = new LinkedList<>();

	@Override
	public List<FriendshipModel> post(FriendshipModel friendship) {
		REPO.add(friendship);
		return REPO;
	}

	@Override
	public List<FriendshipModel> get() {
		return REPO;
	}

	@Override
	public List<FriendshipModel> get(String name) {
		List<FriendshipModel> friendships = new LinkedList<>();

		for (FriendshipModel i : REPO)
			if ((i.getNameFrom().equals(name)) || (i.getNameTo().equals(name)))
				friendships.add(i);

		return friendships;
	}

	@Override
	public FriendshipModel get(String nameFrom, String nameTo) {
		for (FriendshipModel i : REPO)
			if ((i.getNameFrom().equals(nameFrom)) && (i.getNameTo().equals(nameTo)))
				return i;

		return null;
	}

	@Override
	public FriendshipModel put(FriendshipModel friendship) {
		for (FriendshipModel i : REPO)
			if ((i.getNameFrom().equals(friendship.getNameFrom())) && (i.getNameTo().equals(friendship.getNameTo()))) {
				i.setConnectDay(friendship.getConnectDay());

				return i;
			}

		return null;
	}

	@Override
	public List<FriendshipModel> delete(String nameFrom, String nameTo) {
		for (int i = 0; i < REPO.size(); ++i)
			if ((REPO.get(i).getNameFrom().equals(nameFrom)) && (REPO.get(i).getNameTo().equals(nameTo))) {
				REPO.remove(i);
				break;
			}

		return REPO;
	}
}
