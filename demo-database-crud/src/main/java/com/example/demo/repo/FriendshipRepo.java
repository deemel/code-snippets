package com.example.demo.repo;

import com.example.demo.model.FriendshipModel;

public interface FriendshipRepo {
	public Object post(FriendshipModel friendship);

	public Object get();

	public Object get(String name);

	public Object get(String nameFrom, String nameTo);

	public Object put(FriendshipModel friendship);

	public Object delete(String nameFrom, String nameTo);
}
