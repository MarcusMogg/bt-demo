import time
import sys
import json

time.sleep(2)

shell_arg = sys.argv[1]

print(
    json.dumps(
        {
            "str_output": shell_arg,
            "xml_output": r"""<root BTCPP_format="4">
     <BehaviorTree>
		<Sequence>
        	<MoveTo  goal="15,15" />
			<MoveTo  goal="15,50" />
			<Move  dir="right" dis="500" />
		</Sequence>
     </BehaviorTree>
 </root>""",
        }
    )
)
