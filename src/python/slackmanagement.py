import argparse, json
from slackclient import SlackClient

with open('C:\\Users\\bloomberg\\CLionProjects\\bloomberg_backtester\\src\\private\\configuration.json') as f:
    data = json.load(f)

parser = argparse.ArgumentParser()
parser.add_argument('-m', '--message', action='store', dest='message', help='Message to send through Slack.')
args = parser.parse_args()

sc = SlackClient(data['slackKey'])

sc.api_call(
    "chat.postMessage",
    channel="terminal",
    text=args.message
)