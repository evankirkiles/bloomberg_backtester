import argparse
from slackclient import SlackClient

parser = argparse.ArgumentParser()
parser.add_argument('-m', '--message', action='store', dest='message', help='Message to send through Slack.')
args = parser.parse_args()

sc = SlackClient("xoxb-443669380613-546466058231-H5dd72QUWOhuPEiWIFgNfmeB")

sc.api_call(
    "chat.postMessage",
    channel="general",
    text=args.message
)