from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar, Iterable, Mapping, Optional, Union

DEPUTY: NodeRole
DESCRIPTOR: _descriptor.FileDescriptor
DOWN: Direction
HUMAN: PlayerType
LEFT: Direction
MASTER: NodeRole
NORMAL: NodeRole
RIGHT: Direction
ROBOT: PlayerType
UP: Direction
VIEWER: NodeRole

class GameAnnouncement(_message.Message):
    __slots__ = ["can_join", "config", "game_name", "players"]
    CAN_JOIN_FIELD_NUMBER: ClassVar[int]
    CONFIG_FIELD_NUMBER: ClassVar[int]
    GAME_NAME_FIELD_NUMBER: ClassVar[int]
    PLAYERS_FIELD_NUMBER: ClassVar[int]
    can_join: bool
    config: GameConfig
    game_name: str
    players: GamePlayers
    def __init__(self, players: Optional[Union[GamePlayers, Mapping]] = ..., config: Optional[Union[GameConfig, Mapping]] = ..., can_join: bool = ..., game_name: Optional[str] = ...) -> None: ...

class GameConfig(_message.Message):
    __slots__ = ["food_static", "height", "state_delay_ms", "width"]
    FOOD_STATIC_FIELD_NUMBER: ClassVar[int]
    HEIGHT_FIELD_NUMBER: ClassVar[int]
    STATE_DELAY_MS_FIELD_NUMBER: ClassVar[int]
    WIDTH_FIELD_NUMBER: ClassVar[int]
    food_static: int
    height: int
    state_delay_ms: int
    width: int
    def __init__(self, width: Optional[int] = ..., height: Optional[int] = ..., food_static: Optional[int] = ..., state_delay_ms: Optional[int] = ...) -> None: ...

class GameMessage(_message.Message):
    __slots__ = ["ack", "announcement", "discover", "error", "join", "msg_seq", "ping", "receiver_id", "role_change", "sender_id", "state", "steer"]
    class AckMsg(_message.Message):
        __slots__ = []
        def __init__(self) -> None: ...
    class AnnouncementMsg(_message.Message):
        __slots__ = ["games"]
        GAMES_FIELD_NUMBER: ClassVar[int]
        games: _containers.RepeatedCompositeFieldContainer[GameAnnouncement]
        def __init__(self, games: Optional[Iterable[Union[GameAnnouncement, Mapping]]] = ...) -> None: ...
    class DiscoverMsg(_message.Message):
        __slots__ = []
        def __init__(self) -> None: ...
    class ErrorMsg(_message.Message):
        __slots__ = ["error_message"]
        ERROR_MESSAGE_FIELD_NUMBER: ClassVar[int]
        error_message: str
        def __init__(self, error_message: Optional[str] = ...) -> None: ...
    class JoinMsg(_message.Message):
        __slots__ = ["game_name", "player_name", "player_type", "requested_role"]
        GAME_NAME_FIELD_NUMBER: ClassVar[int]
        PLAYER_NAME_FIELD_NUMBER: ClassVar[int]
        PLAYER_TYPE_FIELD_NUMBER: ClassVar[int]
        REQUESTED_ROLE_FIELD_NUMBER: ClassVar[int]
        game_name: str
        player_name: str
        player_type: PlayerType
        requested_role: NodeRole
        def __init__(self, player_type: Optional[Union[PlayerType, str]] = ..., player_name: Optional[str] = ..., game_name: Optional[str] = ..., requested_role: Optional[Union[NodeRole, str]] = ...) -> None: ...
    class PingMsg(_message.Message):
        __slots__ = []
        def __init__(self) -> None: ...
    class RoleChangeMsg(_message.Message):
        __slots__ = ["receiver_role", "sender_role"]
        RECEIVER_ROLE_FIELD_NUMBER: ClassVar[int]
        SENDER_ROLE_FIELD_NUMBER: ClassVar[int]
        receiver_role: NodeRole
        sender_role: NodeRole
        def __init__(self, sender_role: Optional[Union[NodeRole, str]] = ..., receiver_role: Optional[Union[NodeRole, str]] = ...) -> None: ...
    class StateMsg(_message.Message):
        __slots__ = ["state"]
        STATE_FIELD_NUMBER: ClassVar[int]
        state: GameState
        def __init__(self, state: Optional[Union[GameState, Mapping]] = ...) -> None: ...
    class SteerMsg(_message.Message):
        __slots__ = ["direction"]
        DIRECTION_FIELD_NUMBER: ClassVar[int]
        direction: Direction
        def __init__(self, direction: Optional[Union[Direction, str]] = ...) -> None: ...
    ACK_FIELD_NUMBER: ClassVar[int]
    ANNOUNCEMENT_FIELD_NUMBER: ClassVar[int]
    DISCOVER_FIELD_NUMBER: ClassVar[int]
    ERROR_FIELD_NUMBER: ClassVar[int]
    JOIN_FIELD_NUMBER: ClassVar[int]
    MSG_SEQ_FIELD_NUMBER: ClassVar[int]
    PING_FIELD_NUMBER: ClassVar[int]
    RECEIVER_ID_FIELD_NUMBER: ClassVar[int]
    ROLE_CHANGE_FIELD_NUMBER: ClassVar[int]
    SENDER_ID_FIELD_NUMBER: ClassVar[int]
    STATE_FIELD_NUMBER: ClassVar[int]
    STEER_FIELD_NUMBER: ClassVar[int]
    ack: GameMessage.AckMsg
    announcement: GameMessage.AnnouncementMsg
    discover: GameMessage.DiscoverMsg
    error: GameMessage.ErrorMsg
    join: GameMessage.JoinMsg
    msg_seq: int
    ping: GameMessage.PingMsg
    receiver_id: int
    role_change: GameMessage.RoleChangeMsg
    sender_id: int
    state: GameMessage.StateMsg
    steer: GameMessage.SteerMsg
    def __init__(self, msg_seq: Optional[int] = ..., sender_id: Optional[int] = ..., receiver_id: Optional[int] = ..., ping: Optional[Union[GameMessage.PingMsg, Mapping]] = ..., steer: Optional[Union[GameMessage.SteerMsg, Mapping]] = ..., ack: Optional[Union[GameMessage.AckMsg, Mapping]] = ..., state: Optional[Union[GameMessage.StateMsg, Mapping]] = ..., announcement: Optional[Union[GameMessage.AnnouncementMsg, Mapping]] = ..., join: Optional[Union[GameMessage.JoinMsg, Mapping]] = ..., error: Optional[Union[GameMessage.ErrorMsg, Mapping]] = ..., role_change: Optional[Union[GameMessage.RoleChangeMsg, Mapping]] = ..., discover: Optional[Union[GameMessage.DiscoverMsg, Mapping]] = ...) -> None: ...

class GamePlayer(_message.Message):
    __slots__ = ["id", "ip_address", "name", "port", "role", "score", "type"]
    ID_FIELD_NUMBER: ClassVar[int]
    IP_ADDRESS_FIELD_NUMBER: ClassVar[int]
    NAME_FIELD_NUMBER: ClassVar[int]
    PORT_FIELD_NUMBER: ClassVar[int]
    ROLE_FIELD_NUMBER: ClassVar[int]
    SCORE_FIELD_NUMBER: ClassVar[int]
    TYPE_FIELD_NUMBER: ClassVar[int]
    id: int
    ip_address: str
    name: str
    port: int
    role: NodeRole
    score: int
    type: PlayerType
    def __init__(self, name: Optional[str] = ..., id: Optional[int] = ..., ip_address: Optional[str] = ..., port: Optional[int] = ..., role: Optional[Union[NodeRole, str]] = ..., type: Optional[Union[PlayerType, str]] = ..., score: Optional[int] = ...) -> None: ...

class GamePlayers(_message.Message):
    __slots__ = ["players"]
    PLAYERS_FIELD_NUMBER: ClassVar[int]
    players: _containers.RepeatedCompositeFieldContainer[GamePlayer]
    def __init__(self, players: Optional[Iterable[Union[GamePlayer, Mapping]]] = ...) -> None: ...

class GameState(_message.Message):
    __slots__ = ["foods", "players", "snakes", "state_order"]
    class Coord(_message.Message):
        __slots__ = ["x", "y"]
        X_FIELD_NUMBER: ClassVar[int]
        Y_FIELD_NUMBER: ClassVar[int]
        x: int
        y: int
        def __init__(self, x: Optional[int] = ..., y: Optional[int] = ...) -> None: ...
    class Snake(_message.Message):
        __slots__ = ["head_direction", "player_id", "points", "state"]
        class SnakeState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
            __slots__ = []
        ALIVE: GameState.Snake.SnakeState
        HEAD_DIRECTION_FIELD_NUMBER: ClassVar[int]
        PLAYER_ID_FIELD_NUMBER: ClassVar[int]
        POINTS_FIELD_NUMBER: ClassVar[int]
        STATE_FIELD_NUMBER: ClassVar[int]
        ZOMBIE: GameState.Snake.SnakeState
        head_direction: Direction
        player_id: int
        points: _containers.RepeatedCompositeFieldContainer[GameState.Coord]
        state: GameState.Snake.SnakeState
        def __init__(self, player_id: Optional[int] = ..., points: Optional[Iterable[Union[GameState.Coord, Mapping]]] = ..., state: Optional[Union[GameState.Snake.SnakeState, str]] = ..., head_direction: Optional[Union[Direction, str]] = ...) -> None: ...
    FOODS_FIELD_NUMBER: ClassVar[int]
    PLAYERS_FIELD_NUMBER: ClassVar[int]
    SNAKES_FIELD_NUMBER: ClassVar[int]
    STATE_ORDER_FIELD_NUMBER: ClassVar[int]
    foods: _containers.RepeatedCompositeFieldContainer[GameState.Coord]
    players: GamePlayers
    snakes: _containers.RepeatedCompositeFieldContainer[GameState.Snake]
    state_order: int
    def __init__(self, state_order: Optional[int] = ..., snakes: Optional[Iterable[Union[GameState.Snake, Mapping]]] = ..., foods: Optional[Iterable[Union[GameState.Coord, Mapping]]] = ..., players: Optional[Union[GamePlayers, Mapping]] = ...) -> None: ...

class NodeRole(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []

class PlayerType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []

class Direction(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
