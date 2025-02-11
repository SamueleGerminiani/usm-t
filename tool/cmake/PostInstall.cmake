#install third_party libraries
file(COPY ${SpotLib} DESTINATION ${IP}/usm-t/lib FOLLOW_SYMLINK_CHAIN)
file(COPY ${BddxLib} DESTINATION ${IP}/usm-t/lib FOLLOW_SYMLINK_CHAIN)
file(COPY ${AntrLib} DESTINATION ${IP}/usm-t/lib FOLLOW_SYMLINK_CHAIN)
file(COPY ${BoostLib} DESTINATION ${IP}/usm-t/lib FOLLOW_SYMLINK_CHAIN)
file(COPY ${z3Lib} DESTINATION ${IP}/usm-t/lib FOLLOW_SYMLINK_CHAIN)
