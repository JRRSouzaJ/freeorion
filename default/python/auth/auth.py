from logging import info, warning

from common.configure_logging import redirect_logging_to_freeorion_logger

# Logging is redirected before other imports so that import errors appear in log files.
redirect_logging_to_freeorion_logger()

import freeorion as fo
import sys

# Constants defined by the C++ game engine
NO_TEAM_ID = -1


class AuthProvider:
    def __init__(self):
        self.logins = {}
        self.roles_symbols = {
            "h": fo.roleType.host,
            "m": fo.roleType.clientTypeModerator,
            "p": fo.roleType.clientTypePlayer,
            "o": fo.roleType.clientTypeObserver,
            "g": fo.roleType.galaxySetup,
        }
        try:
            with open(fo.get_user_config_dir() + "/auth.txt") as f:
                first_line = True
                for line in f:
                    if first_line:
                        first_line = False
                        self.default_roles = self.__parse_roles(line.strip())
                    else:
                        login, roles, password = line.rsplit(":", 2)
                        self.logins[login] = (password.strip(), self.__parse_roles(roles.strip()))
        except IOError:
            exctype, value = sys.exc_info()[:2]
            warning("Can't read auth file %s: %s %s" % (fo.get_user_config_dir() + "/auth.txt", exctype, value))
            self.default_roles = [
                fo.roleType.clientTypeModerator,
                fo.roleType.clientTypePlayer,
                fo.roleType.clientTypeObserver,
                fo.roleType.galaxySetup,
            ]
        info("Auth initialized")

    def __parse_roles(self, roles_str):
        roles = []
        for c in roles_str:
            r = self.roles_symbols.get(c)
            if r is None:
                warning("unknown role symbol '%c'" % c)
            else:
                roles.append(r)
        return roles

    def is_require_auth_or_return_roles(self, player_name):
        """Returns True if player should be authenticated, False if user not allowed,
        or list of roles for anonymous players"""
        known_login = player_name in self.logins
        if not known_login:
            # default list of roles
            return self.default_roles
        return True

    def is_success_auth_and_return_roles(self, player_name, auth):
        """Return False if passowrd doesn't match or list of roles for authenticated player"""
        auth_data = self.logins.get(player_name)
        if auth_data[0] == auth:
            return auth_data[1]
        else:
            return False

    def list_players(self):
        """Returns list of PlayerSetupData to use in quickstart"""
        players = []
        for player_name, auth_data in self.logins.items():
            if fo.roleType.clientTypePlayer in auth_data[1]:
                psd = fo.PlayerSetupData()
                psd.player_name = player_name
                psd.empire_name = player_name
                psd.starting_species = "RANDOM"
                psd.starting_team = NO_TEAM_ID
                players.append(psd)
        return players

    def get_player_delegation(self, player_name):
        """Returns list of players delegated by this player"""
        return []
