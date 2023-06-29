#!/bin/bash


if [ -n "$1" ]
then
  ln cmake-build-debug/3b "$1"
  if [ "$1" == "setPerms"  ]
  then
    if [ -n "$3" ]
    then
      ./setPerms "$2" "$3"
    fi
  elif [ -n "$2" ]; then
      "./$1" "$2"
  fi
  rm "./$1"
fi